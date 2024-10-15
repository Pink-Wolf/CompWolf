using CompWolf.Docs.Server.Models;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Xml.Linq;

namespace CompWolf.Docs.Server.Data
{
    public class SourceDatabase
    {
        public static string CompWolfProgramPath => _compWolfProgramPath;
        private static readonly string _compWolfProgramPath = GetCompWolfProgramPath();
        private static string GetCompWolfProgramPath()
        {
            var parentDirectory = Directory.GetCurrentDirectory();
            var targetDirectory = "CompWolf.Programs";
            while (Directory.Exists(Path.Combine(parentDirectory, targetDirectory)) is false)
            {
                parentDirectory = Directory.GetParent(parentDirectory)?.FullName;
                if (parentDirectory == null) throw new DirectoryNotFoundException($"Could not find directory \"{targetDirectory}\" for CompWolf headers");
            }
            return Path.Combine(parentDirectory, targetDirectory);
        }


        public async Task<SourceCollection> GetSourceCollection()
        {
            List<SourceProject> projects = [];
            await foreach (var project in GetProjects())
                projects.Add(project);

            return new()
            {
                Projects = [.. projects],
            };

        }

        static async IAsyncEnumerable<SourceProject> GetProjects()
        {
            var projectNames = await GetProjectNames();

            var headers = new List<IAsyncEnumerable<SourceHeader>>(projectNames.Length);
            for (int i = 0; i < projectNames.Length; ++i)
            {
                var headerPath = Path.Combine(CompWolfProgramPath, projectNames[i], "include");
                if (Directory.Exists(headerPath))
                    headers.Add(GetHeaders(headerPath));
            }

            for (int i = 0; i < headers.Count; ++i)
            {
                List<SourceHeader> projectHeaders = [];
                await foreach (var header in headers[i])
                    projectHeaders.Add(header);

                yield return new()
                {
                    Name = "CompWolf." + projectNames[i],
                    Headers = [.. projectHeaders],
                };
            }
        }
        static async Task<string[]> GetProjectNames()
        {
            var parentMakeListPath = Path.Combine(CompWolfProgramPath, "CMakeLists.txt");
            var parentMakeListData = await File.ReadAllTextAsync(parentMakeListPath);

            {
                var searchString = "SUBPROJECTS";
                parentMakeListData = parentMakeListData[(parentMakeListData.IndexOf(searchString) + searchString.Length)..];
            }
            parentMakeListData = parentMakeListData[..(parentMakeListData.IndexOf(')'))];

            return parentMakeListData.Split(Array.Empty<char>(), StringSplitOptions.RemoveEmptyEntries)
                .Select(x => x[1..(x.Length - 1)])
                .ToArray();
        }

        static async IAsyncEnumerable<SourceHeader> GetHeaders(string headerPath)
        {
            var headerFiles = Directory.GetFiles(headerPath);

            foreach (var headerFile in headerFiles)
            {
                List<string> descriptions = [];
                List<string> files = [];

                await foreach (var line in File.ReadLinesAsync(headerFile))
                {
                    if (line.StartsWith("//"))
                        descriptions.Add(line[2..]);
                    else if (line.StartsWith("#include \"private/"))
                    {
                        var fileLine = line[10..];
                        files.Add(Path.Combine(headerPath, fileLine[..fileLine.IndexOf('"')]));
                    }
                }

                yield return new SourceHeader()
                {
                    Name = Path.GetFileNameWithoutExtension(headerFile),
                    BriefDescription = string.Join(Environment.NewLine, descriptions),
                    Entities = (await Task.WhenAll(files.Select(GetEntitiesFromFile)))
                        .SelectMany(x => x)
                        .ToArray()
                };
            }

        }

        private readonly static ConcurrentDictionary<string, (SourceEntity[] data, DateTime cacheTime)> _entityCache = [];
        static async Task<SourceEntity[]> GetEntitiesFromFile(string filePath)
        {
            var fileTime = File.GetLastWriteTimeUtc(filePath);

            if (false && _entityCache.TryGetValue(filePath, out var cachedEntities))
            {
                if (fileTime.Equals(cachedEntities.cacheTime))
                {
                    return cachedEntities.data;
                }
            }

            var fileData = await File.ReadAllTextAsync(filePath);
            var data = SplitTextIntoNamespaces(fileData)
                .SelectMany(x => GetEntities(x.Item2, x.Item1))
                .ToArray();

            _entityCache[filePath] = (data, fileTime);

            return data;
        }
        static IEnumerable<SourceEntity> GetEntities(string text, string namespaceName)
        {
            foreach (var entityText in SplitMemberGroupIntoMember(text))
            {
                var entityComment = entityText.Comment;
                var entityDeclaration = entityText.Declaration;
                var entityBody = entityText.Body;

                var templateMatch = Regex.Match(entityDeclaration, @"\s*template\s*<");
                var templateEndIndex = templateMatch.Success
                    ? GetEndOfBracketIndex('<', '>', entityDeclaration, templateMatch.Index)
                    : -1;
                var noTemplateDeclaration = entityDeclaration[(templateEndIndex + 1)..];

                var parameterLessDeclaration = noTemplateDeclaration;
                bool hasParameters;
                {
                    var parameterIndex = parameterLessDeclaration.IndexOf('(');
                    hasParameters = parameterIndex >= 0;
                    if (hasParameters)
                        parameterLessDeclaration = parameterLessDeclaration[..parameterIndex];
                }
                var attributeLessDeclaration = parameterLessDeclaration;
                bool hasAttributes;
                {
                    var parameterIndex = parameterLessDeclaration.IndexOf('[');
                    hasAttributes = parameterIndex >= 0;
                    if (hasAttributes)
                        attributeLessDeclaration = parameterLessDeclaration[..parameterIndex]
                            + parameterLessDeclaration[parameterLessDeclaration.IndexOf(']', parameterIndex)..];
                }

                var cleanedDeclarationWords = parameterLessDeclaration
                    .Split(Array.Empty<char>(), StringSplitOptions.RemoveEmptyEntries)
                    .ToList();

                IEnumerable <SourceEntity> nestedEntities = [];

                bool IsType(string type) => cleanedDeclarationWords[0] == type;
                bool isStruct = IsType("struct");
                var entityType = isStruct ? EntityTypes.Class
                    : IsType("class") ? EntityTypes.Class
                    : IsType("concept") ? EntityTypes.Concept
                    : IsType("enum") ? EntityTypes.Enum
                    : IsType("using") ? EntityTypes.Alias
                    : IsType("#define") ? EntityTypes.Macro
                    : hasParameters ? EntityTypes.Function
                    : EntityTypes.Variable;

                if (entityType == EntityTypes.Class)
                {
                    nestedEntities = SplitIntoMemberGroups(entityBody, isStruct)
                        .Where(x => x.Key.visibility != "private")
                        .SelectMany(x => GetEntities(x.Value, namespaceName))
                        .GroupBy(x => x.Name)
                        .Select(entityEnumerator =>
                        {
                            var entityGroup = entityEnumerator.ToList();
                            var entity = entityGroup.First();

                            if (entityGroup.Count == 1) return entity;

                            for (int i = 1; string.IsNullOrEmpty(entity.BriefDescription) && i < entityGroup.Count; ++i)
                            {
                                entity.BriefDescription = entityGroup[i].BriefDescription;
                            }

                            entity.Declarations = entityGroup.SelectMany(x => x.Declarations).ToArray();

                            return entity;
                        });
                }

                var entityName = entityType switch
                {
                    EntityTypes.Class => cleanedDeclarationWords[1],
                    EntityTypes.Function => cleanedDeclarationWords.Last(),
                    EntityTypes.Macro => cleanedDeclarationWords.Last(),
                    EntityTypes.Alias => cleanedDeclarationWords.Last(),
                    EntityTypes.Concept => cleanedDeclarationWords.Last(),
                    EntityTypes.Variable => cleanedDeclarationWords.Last(),
                    _ => throw new ArgumentOutOfRangeException(nameof(text),
                        "Tried deserializing entity of unknown type")
                };

                    string mainComment, briefComment, returnComment, throwComment;
                Dictionary<string, string> parameterComments;
                {
                    List<string> mainCommentLines = [];
                    List<string> briefCommentLines = [];
                    Dictionary<string, List<string>> parameterCommentLines = [];
                    List<string> returnCommentLines = [];
                    List<string> throwCommentLines = [];
                    {
                        var commentTarget = mainCommentLines;
                        foreach (var line in entityComment.Split(Environment.NewLine))
                        {
                            var newCommentText = line.TrimStart(' ', '*').TrimEnd();
                            var sectionMatch = Regex.Match(line, @"^@(\w+)\s");
                            if (sectionMatch.Success)
                            {
                                newCommentText = newCommentText[sectionMatch.Length..];

                                switch (sectionMatch.Groups[1].Value)
                                {
                                    case "param":
                                        var splitCommentText = newCommentText.Split(' ', 1);
                                        newCommentText = splitCommentText[1].TrimStart();
                                        commentTarget = parameterCommentLines[splitCommentText[0]] = [];
                                        break;
                                    case "return":
                                        commentTarget = returnCommentLines;
                                        break;
                                    case "brief":
                                        commentTarget = briefCommentLines;
                                        break;
                                    case "throws":
                                        commentTarget = throwCommentLines;
                                        break;
                                    default:
                                        throw new FormatException($"Cannot deserialize Javadoc tag {sectionMatch.Groups[1]}");
                                }
                            }

                            commentTarget.Add(line.Trim());
                        }
                    }

                    mainComment = string.Join(Environment.NewLine, mainCommentLines);
                    briefComment = string.Join(Environment.NewLine, briefCommentLines);
                    returnComment = string.Join(Environment.NewLine, returnCommentLines);
                    throwComment = string.Join(Environment.NewLine, throwCommentLines);
                    parameterComments = parameterCommentLines.Select(x => (x.Key, string.Join(Environment.NewLine, x.Value)))
                        .ToDictionary();
                }

                switch (entityType)
                {
                    case EntityTypes.Class:
                    case EntityTypes.Concept:
                    case EntityTypes.Function:
                    case EntityTypes.Variable:
                    case EntityTypes.Enum:
                    case EntityTypes.Alias:
                        if (entityDeclaration.EndsWith(';') is false)
                            entityDeclaration += ';';
                        break;
                    default: break;
                }


                if (entityName.Contains("::") is false)
                {
                    yield return new()
                    {
                        Name = entityName,
                        Type = entityType,
                        BriefDescription =  briefComment,
                        Declarations = [new()
                        {
                            Description = mainComment,
                            Declaration = entityDeclaration,
                        }],
                        ReturnDescription = returnComment,
                        ParameterDescriptions = parameterComments,
                        ThrowDescription = throwComment,
                        Members = nestedEntities.ToArray(),
                        Namespace = namespaceName,
                    };
                }
            }
        }

        class MemberString
        {
            public string Comment { get; set; } = "";
            public string Declaration { get; set; } = "";
            public string Body { get; set; } = "";
        }
        static IEnumerable<MemberString> SplitMemberGroupIntoMember(string text)
        {
            if (string.IsNullOrEmpty(text)) yield break;

            for (int declarationIndex = (text[0] == '{') ? 1 : 0; declarationIndex < text.Length; ++declarationIndex)
            {
                int bodyIndex, endIndex;
                {
                    var firstChar = text[declarationIndex];

                    if (char.IsWhiteSpace(firstChar)) continue;
                    if (firstChar == '{')
                    {
                        declarationIndex = GetEndOfBracketIndex('{', '}', text, firstChar);
                        continue;
                    }
                    if (firstChar == '(')
                    {
                        declarationIndex = GetEndOfBracketIndex('(', ')', text, firstChar);
                        continue;
                    }
                    if (firstChar == '/')
                    {
                        switch (text[declarationIndex + 1])
                        {
                            case '/':
                                declarationIndex = text.IndexOf(Environment.NewLine, declarationIndex + 2);
                                continue;
                            case '*':
                                declarationIndex = text.IndexOf("*/", declarationIndex + 2) + 2;
                                continue;
                            default: break;
                        }
                    }

                    bodyIndex = declarationIndex + 1;
                    var macroMatch = Regex.Match(text[bodyIndex..], @"^\s*#define\s+");
                    if (macroMatch.Success)
                    {
                        bodyIndex += macroMatch.Length;

                        var parenthesisMatch = Regex.Match(text[bodyIndex..], @"^\s\(");
                        if (parenthesisMatch.Success)
                        {
                            bodyIndex = GetEndOfBracketIndex('(', ')', text, bodyIndex + parenthesisMatch.Length - 1);
                        }

                        endIndex = text.IndexOf(Environment.NewLine, bodyIndex);
                    }
                    else
                    {
                        for (; bodyIndex < text.Length; ++bodyIndex)
                        {
                            switch (text[bodyIndex])
                            {
                                case ';':
                                    ++bodyIndex; // no body, so ";" is part of declaration
                                    break;
                                case '{': break;
                                case '=':
                                    if (Regex.Match(text[..bodyIndex], @"\soperator\s*$").Success)
                                        continue; // "=" in method name "operator=" is not the end
                                    else
                                        break;
                                case '<':
                                    bodyIndex = GetEndOfBracketIndex('<', '>', text, bodyIndex);
                                    continue;
                                case '(':
                                    bodyIndex = GetEndOfBracketIndex('(', ')', text, bodyIndex);
                                    continue;
                                default: continue;
                            }
                            break;
                        }
                        if (bodyIndex == text.Length)
                            throw new FormatException($"Cannot find end of member ${text[declarationIndex..Math.Min(text.Length, declarationIndex + 32)]}...");

                        endIndex = bodyIndex;
                        if (text[endIndex] == '{') endIndex = GetEndOfBracketIndex('{', '}', text, endIndex);
                        if (text[endIndex] == '=') endIndex = text.IndexOf(';', bodyIndex + 1) + 1;
                    }
                }

                {
                    MemberString member = new()
                    {
                        Comment = text[0..declarationIndex],
                        Declaration = text[declarationIndex..bodyIndex],
                        Body = text[bodyIndex..endIndex],
                    };

                    int previousWordEndIndex;
                    for (previousWordEndIndex = declarationIndex - 1;
                        previousWordEndIndex > 0 && char.IsWhiteSpace(text[previousWordEndIndex]);
                        --previousWordEndIndex)
                    { }

                    if (previousWordEndIndex > 0)
                    {
                        int previousLineIndex = text.LastIndexOf(Environment.NewLine, previousWordEndIndex);
                        previousLineIndex = previousLineIndex < 0
                            ? 0
                            : previousLineIndex + Environment.NewLine.Length;
                        var firstLine = text[previousLineIndex..(previousWordEndIndex + 1)].TrimStart();

                        if (firstLine.StartsWith("//"))
                        {
                            member.Comment = firstLine[2..];
                        }
                        else if (text[(previousWordEndIndex - 1)..(previousWordEndIndex + 1)] == "*/")
                        {
                            var commentIndex = text.LastIndexOf("/*", previousWordEndIndex - 1);
                            member.Comment = text[(commentIndex + 1)..(previousWordEndIndex - 1)];
                        }
                    }

                    member.Comment = member.Comment.TrimEnd();
                    member.Declaration = member.Declaration.TrimEnd();
                    member.Body = member.Body.TrimEnd();
                    yield return member;
                }

                declarationIndex = endIndex;
            }
        }
        static IEnumerable<(string, string)> SplitTextIntoNamespaces(string text)
        {
            //After search, paranthesisLevel becomes endIndex
            LinkedList<(string name, int startIndex, int paranthesisLevel, List<string> subnamespaces)> namespaceData = [];
            int namespaceWithoutEndCount = 0;

            for (int i = text.IndexOf('{'); i >= 0; i = text.IndexOfAny(['{', '}'], i + 1))
            {
                if (text[i] == '{')
                {
                    var namespaceMatch = Regex.Match(text[0..i], @"namespace\s+([\w\:]+)\s*$");
                    if (namespaceMatch.Success)
                    {
                        var name = namespaceMatch.Groups[1].Value;
                        if (namespaceWithoutEndCount > 0)
                        {
                            name = $"{namespaceData.Last!.Value.name}::{name}";
                            namespaceData.Last!.ValueRef.subnamespaces.Add(name);
                        }

                        namespaceData.AddLast((name, i, 0, []));
                        ++namespaceWithoutEndCount;
                    }
                    else
                    {
                        if (namespaceWithoutEndCount > 0)
                            ++namespaceData.Last!.ValueRef.paranthesisLevel;
                    }
                }
                else
                {
                    if (namespaceData.Last != null)
                    {
                        --namespaceData.Last.ValueRef.paranthesisLevel;
                        var (name, startIndex, paranthesisLevel, subnamespaces) = namespaceData.Last.Value;
                        if (paranthesisLevel < 0)
                        {
                            namespaceData.AddFirst((name, startIndex, i, subnamespaces));
                            namespaceData.RemoveLast();
                            --namespaceWithoutEndCount;
                        }
                    }
                }
            }
            if (namespaceWithoutEndCount > 0)
                throw new FormatException($"Could not find end of namespace (}})");

            return namespaceData
                .Select(space =>
                {
                    var name = space.name;
                    var body = text[(space.startIndex + 1)..space.paranthesisLevel];
                    var subspaces = space.subnamespaces
                        .Select(subspaceName =>
                        {
                            var subspace = namespaceData.First(x => x.name == subspaceName);
                            return (subspace.startIndex, subspace.paranthesisLevel);
                        }
                    );
                    var indexOffset = space.startIndex;
                    foreach (var (startIndex, endIndex) in subspaces)
                    {
                        var start = startIndex - indexOffset;
                        var end = endIndex - indexOffset;
                        body = body[0..start] + body[end..];
                        indexOffset += endIndex - startIndex;
                    }
                    return (name, body);
                });

        }
        static Dictionary<(string groupName, string visibility), string> SplitIntoMemberGroups
            (string text, bool publicByDefault)
        {
            var visibilityPattern = @"(public|protected|private):";
            Dictionary<string, string> groups = [];
            {
                var textSplit = Regex.Split(text, @"({)|(})|" + visibilityPattern + @"\s*\/\/\s*(.+)");

                var currentGroup = "";
                groups.Add(currentGroup, (publicByDefault ? "public:" : "private:") + textSplit[2]);

                for (int i = 3; i < textSplit.Length; ++i)
                {
                    switch (textSplit[i])
                    {
                        case "{":
                            var newI = GetEndOfBracketIndex("{", "}", textSplit, i);
                            groups[currentGroup] = string.Join(null, [groups[currentGroup], .. textSplit[i..(newI + 1)]]);
                            i = newI;
                            continue;
                        case "}": break;
                        default:
                            var visibility = textSplit[i];
                            currentGroup = textSplit[++i].Trim();
                            var body = textSplit[++i];
                            groups[currentGroup] = visibility + ':' + body;
                            continue;
                    }
                    break;
                }
            }

            Dictionary<(string groupName, string visibility), string> results = [];
            foreach (var (groupName, groupBody) in groups)
            {
                results[(groupName, "public")] = "";
                results[(groupName, "protected")] = "";
                results[(groupName, "private")] = "";

                var sections = Regex.Split(groupBody, visibilityPattern);
                for (int i = 1; i < sections.Length; i += 2)
                {
                    var sectionVisibility = sections[i];
                    var sectionBody = sections[i + 1];
                    results[(groupName, sectionVisibility)] += sectionBody;
                }
            }

            return results;
        }

        /// <param name="startIndex"> The index of the start bracket to find the end of. </param>
        static int GetEndOfBracketIndex<Char, String>(Char startBracket, Char endBracket, String text, int startIndex = 0)
            where Char : IEquatable<Char>
            where String : IEnumerable<Char>
        {
            int level = 0;
            int count = startIndex;
            var iterator = text.Skip(startIndex).GetEnumerator();

            for (; iterator.MoveNext(); ++count)
            {
                var c = iterator.Current;

                if (startBracket.Equals(c))
                    ++level;
                if (endBracket.Equals(c))
                {
                    --level;
                    if (level == 0)
                        return count + 1;
                }
            }
            throw new FormatException($"Could not find {endBracket} to pair with {startBracket}");
        }
        /// <param name="startIndex"> The index of the end bracket to find the end of. </param>
        static int GetEndOfBracketIndexReverse<Char, String>(Char startBracket, Char endBracket, String text, int startIndex)
            where Char : IEquatable<Char>
            where String : IEnumerable<Char>
            => startIndex - GetEndOfBracketIndex(endBracket, startBracket, text.Take(startIndex + 1).Reverse());
    }
}
