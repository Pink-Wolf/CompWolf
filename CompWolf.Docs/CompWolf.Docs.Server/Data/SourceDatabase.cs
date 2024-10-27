using CompWolf.Docs.Server.Models;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Xml.Linq;

namespace CompWolf.Docs.Server.Data
{
    public class SourceDatabase
    {
        public static readonly string Newline = "\r";
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
                    BriefDescription = string.Join(Newline, descriptions),
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
            var data = Namespace.SplitText(fileData)
                .SelectMany(GetEntitiesFromNamespace)
                .ToArray();

            _entityCache[filePath] = (data, fileTime);

            return data;
        }
        static IEnumerable<SourceEntity> GetEntitiesFromNamespace(Namespace space)
        {
            var namespaceName = space.Name;
            var text = space.Text;

            foreach (var entityText in MemberEntity.SplitText(text))
            {
                var entityComment = entityText.Comment;
                var entityDeclaration = entityText.Declaration;
                var entityBody = entityText.Body;

                var templateMatch = Regex.Match(entityDeclaration, @"\s*template\s*<");
                var templateEndIndex = templateMatch.Success
                    ? GetEndOfBracketIndexInCode('<', '>', entityDeclaration, templateMatch.Index)
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

                Dictionary<string, SourceEntity[]> nestedEntities = [];
                SourceEntity? constructor = null;

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

                var entityName = entityType switch
                {
                    EntityTypes.Class => cleanedDeclarationWords[1],
                    EntityTypes.Function => cleanedDeclarationWords.Last(),
                    EntityTypes.Macro => cleanedDeclarationWords.Last(),
                    EntityTypes.Alias => cleanedDeclarationWords.Last(),
                    EntityTypes.Concept => cleanedDeclarationWords.Last(),
                    EntityTypes.Variable => cleanedDeclarationWords.Last(),
                    _ => throw new ArgumentOutOfRangeException(nameof(space),
                        "Tried deserializing entity of unknown type")
                };

                if (entityType == EntityTypes.Class)
                {
                    nestedEntities = MemberGroup.SplitText(entityBody, isStruct)
                        .Select(x => (x.GroupName, x.TextPerVisibility
                                .Where(x => x.Key != "private")
                                .SelectMany(x => GetEntitiesFromNamespace(new()
                                {
                                    Name = namespaceName,
                                    Text = x.Value,
                                })
                                    .Select(Value => (x.Key, Value)))
                                .GroupBy(x => x.Value.Name)
                                .Select(entityEnumerator =>
                                {
                                    var entityGroup = entityEnumerator.ToList();
                                    var entity = entityGroup.First().Value;

                                    if (entityGroup.Count == 1) return entity;

                                    for (int i = 1; string.IsNullOrEmpty(entity.BriefDescription) && i < entityGroup.Count; ++i)
                                    {
                                        entity.BriefDescription = entityGroup[i].Value.BriefDescription;
                                    }

                                    entity.Declarations = entityGroup.SelectMany(x => x.Value.Declarations).ToArray();

                                    if (entityGroup.First().Key == "protected")
                                        foreach (var declaration in entity.Declarations)
                                            declaration.Protected = true;

                                    return entity;
                                })
                                .ToArray()
                            ))
                            .ToDictionary();

                    if (nestedEntities.TryGetValue("constructors", out var constructors))
                    {
                        nestedEntities.Remove("constructors");
                        constructor = CombineEntities(constructors);
                        if (constructor is not null)
                        {
                            constructor.Name = entityName;
                            constructor.Type = EntityTypes.Function;
                        }
                    }
                }

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
                        foreach (var line in entityComment.Split(Newline))
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

                    mainComment = string.Join(Newline, mainCommentLines);
                    briefComment = string.Join(Newline, briefCommentLines);
                    returnComment = string.Join(Newline, returnCommentLines);
                    throwComment = string.Join(Newline, throwCommentLines);
                    parameterComments = parameterCommentLines.Select(x => (x.Key, string.Join(Newline, x.Value)))
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
                        Namespace = namespaceName,
                        Constructors = constructor,
                        Members = nestedEntities,
                        ReturnDescription = returnComment,
                        ParameterDescriptions = parameterComments,
                        ThrowDescription = throwComment,
                    };
                }
            }
        }

        public class MemberEntity
        {
            public string Comment { get; set; } = "";
            public string Declaration { get; set; } = "";
            public string Body { get; set; } = "";

            public static IEnumerable<MemberEntity> SplitText(string text)
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
                            declarationIndex = GetEndOfBracketIndexInCode('{', '}', text, firstChar);
                            continue;
                        }
                        if (firstChar == '(')
                        {
                            declarationIndex = GetEndOfBracketIndexInCode('(', ')', text, firstChar);
                            continue;
                        }
                        if (firstChar == '/')
                        {
                            switch (text[declarationIndex + 1])
                            {
                                case '/':
                                    declarationIndex = text.IndexOf(Newline, declarationIndex + 2);
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
                                bodyIndex = GetEndOfBracketIndexInCode('(', ')', text, bodyIndex + parenthesisMatch.Length - 1);
                            }

                            endIndex = text.IndexOf(Newline, bodyIndex);
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
                                        bodyIndex = GetEndOfBracketIndexInCode('<', '>', text, bodyIndex);
                                        continue;
                                    case '(':
                                        bodyIndex = GetEndOfBracketIndexInCode('(', ')', text, bodyIndex);
                                        continue;
                                    default: continue;
                                }
                                break;
                            }
                            if (bodyIndex == text.Length)
                                throw new FormatException($"Cannot find end of member ${text[declarationIndex..Math.Min(text.Length, declarationIndex + 32)]}...");

                            endIndex = bodyIndex;
                            if (text[endIndex] == '{') endIndex = GetEndOfBracketIndexInCode('{', '}', text, endIndex);
                            if (text[endIndex] == '=') endIndex = text.IndexOf(';', bodyIndex + 1) + 1;
                        }
                    }

                    {
                        MemberEntity member = new()
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
                            int previousLineIndex = text.LastIndexOf(Newline, previousWordEndIndex);
                            previousLineIndex = previousLineIndex < 0
                                ? 0
                                : previousLineIndex + Newline.Length;
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
        }

        public class MemberGroup
        {
            public string GroupName { get; set; } = "";
            public Dictionary<string, string> TextPerVisibility { get; set; } = [];

            public static IEnumerable<MemberGroup> SplitText(string text, bool publicByDefault)
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

                return groups
                    .Select(text =>
                    {
                        var result = new MemberGroup()
                        {
                            GroupName = text.Key,
                            TextPerVisibility = new(),
                        };
                        result.TextPerVisibility.Add("public", "");
                        result.TextPerVisibility.Add("protected", "");
                        result.TextPerVisibility.Add("private", "");

                        var sections = Regex.Split(text.Value, visibilityPattern);
                        for (int i = 1; i < sections.Length; i += 2)
                        {
                            var sectionVisibility = sections[i];
                            var sectionBody = sections[i + 1];
                            result.TextPerVisibility[sectionVisibility] += sectionBody;
                        }
                        return result;
                    });
            }
        }
        public class Namespace
        {
            public string Name { get; set; } = "";
            public string Text { get; set; } = "";

            public override string ToString()
                => $"{Name}: {Text}";
            public override bool Equals(object? obj)
                => obj is Namespace other
                && Name.Equals(other.Name)
                && Text.Equals(other.Text);

            public static IEnumerable<Namespace> SplitText(string text)
            {
                //After search, paranthesisLevel becomes endIndex
                LinkedList<(string name, int nameIndex, int startIndex, int paranthesisLevel, List<string> subnamespaces)> namespaceData = [];
                int namespaceWithoutEndCount = 0;

                foreach (var i in ForeachRelevantCharInCode(text))
                {
                    switch (text[i])
                    {
                        case '{':
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

                                    namespaceData.AddLast((name, namespaceMatch.Index, i, 0, []));
                                    ++namespaceWithoutEndCount;
                                }
                                else
                                {
                                    if (namespaceWithoutEndCount > 0)
                                        ++namespaceData.Last!.ValueRef.paranthesisLevel;
                                }
                            }
                            break;
                        case '}':
                            {
                                if (namespaceData.Last != null)
                                {
                                    --namespaceData.Last.ValueRef.paranthesisLevel;
                                    var (name, nameIndex, startIndex, paranthesisLevel, subnamespaces) = namespaceData.Last.Value;
                                    if (paranthesisLevel < 0)
                                    {
                                        namespaceData.AddFirst((name, nameIndex, startIndex, i, subnamespaces));
                                        namespaceData.RemoveLast();
                                        --namespaceWithoutEndCount;
                                    }
                                }
                            }
                            break;
                        default: break;
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
                                return (subspace.nameIndex, subspace.paranthesisLevel);
                            }
                        );
                        var indexOffset = space.startIndex;
                        foreach (var (nameIndex, endIndex) in subspaces)
                        {
                            var startIndex = nameIndex - 2;
                            var start = startIndex - indexOffset;
                            var end = endIndex - indexOffset;
                            body = body[0..start] + body[end..];
                            indexOffset += endIndex - startIndex;
                        }
                        return new Namespace()
                        {
                            Name = name,
                            Text = body,
                        };
                    });
            }
        }

        /// <param name="startIndex"> The index of the start bracket to find the end of. </param>
        public static int GetEndOfBracketIndex<Char, String>(Func<Char, bool> isStartBracket, Func<Char, bool> isEndBracket, String text, int startIndex = 0)
            where Char : IEquatable<Char>
            where String : IEnumerable<Char>
        {
            int level = 0;
            int count = startIndex;
            var iterator = (startIndex <= 0)
                ? text.GetEnumerator()
                : text.Skip(startIndex).GetEnumerator();

            if (iterator.MoveNext() is false
                || isStartBracket(iterator.Current) is false)
                throw new ArgumentException($"Could not find start-bracket at index {startIndex} of {text}");


            for (; iterator.MoveNext(); ++count)
            {
                var c = iterator.Current;

                if (isStartBracket(c))
                    ++level;
                else if (isEndBracket(c))
                {
                    --level;
                    if (level < 0)
                        return count + 1;
                }
            }

            throw new FormatException($"Could not find end-bracket to match start-bracket at index {startIndex} of ${text}");
        }
        /// <param name="startIndex"> The index of the start bracket to find the end of. </param>
        public static Char GetEndOfBracket<Char, String>(Func<Char, bool> isStartBracket, Func<Char, bool> isEndBracket, String text, int startIndex = 0)
            where Char : IEquatable<Char>
            where String : IEnumerable<Char>
        {
            int level = 0;
            var iterator = (startIndex <= 0)
                ? text.GetEnumerator()
                : text.Skip(startIndex).GetEnumerator();

            if (iterator.MoveNext() is false
                || isStartBracket(iterator.Current) is false)
                throw new ArgumentException($"Could not find start-bracket at index {startIndex} of {text}");


            while (iterator.MoveNext())
            {
                var c = iterator.Current;

                if (isStartBracket(c))
                    ++level;
                else if (isEndBracket(c))
                {
                    --level;
                    if (level < 0)
                        return c;
                }
            }

            throw new FormatException($"Could not find end-bracket to match start-bracket at index {startIndex} of {text}");
        }
        /// <param name="startIndex"> The index of the start bracket to find the end of. </param>
        public static int GetEndOfBracketIndex<Char, String>(Char startBracket, Char endBracket, String text, int startIndex = 0)
            where Char : IEquatable<Char>
            where String : IEnumerable<Char>
            => GetEndOfBracketIndex<Char, String>(startBracket.Equals, endBracket.Equals, text, startIndex);

        public static int GetEndOfBracketIndexInCode(char startBracket, char endBracket, string text, int startIndex = 0)
            => GetEndOfBracket<int, IEnumerable<int>>(i => text[i] == startBracket, i => text[i] == endBracket, ForeachRelevantCharInCode(text, startIndex), 0);

        public static IEnumerable<int> ForeachRelevantCharInCode(string text, int startIndex = 0)
        {
            for (var index = startIndex; index < text.Length;)
            {
                var oldIndex = index;
                switch (text[index])
                {
                    case '"':
                        if ((index > 0) && (text[index - 1] == 'R'))
                        {
                            ++index;
                            var endString = text[index..text.IndexOf('(', index)];
                            endString = $"){endString}\"";
                            index = text.IndexOf(endString, index + endString.Length - 1);
                            if (index < 0)
                                throw new FormatException($"Could not find ${endString} after index {oldIndex} of {text}");
                            index += endString.Length;
                        }
                        else
                        {
                            do
                            {
                                index = text.IndexOf('"', ++index);
                                if (index < 0)
                                    throw new FormatException($"Could not find \" after index {oldIndex} of {text}");
                                index += 1;
                            } while (text[index - 2] == '\\');
                        }
                        break;
                    case '/':
                        switch (text[++index])
                        {
                            case '/':
                                {
                                    index = text.IndexOf(Newline, ++index);
                                    if (index < 0)
                                        yield break;
                                    index += Newline.Length;
                                }
                                break;
                            case '*':
                                {
                                    index = text.IndexOf("*/", ++index);
                                    if (index < 0)
                                        throw new FormatException($"Could not find */ after index {oldIndex} of {text}");
                                    index += 2;
                                }
                                break;
                            default:
                                yield return index;
                                ++index;
                                break;
                        }
                        break;
                    default:
                        yield return index;
                        ++index;
                        break;
                }
            }
        }

        public static SourceEntity? CombineEntities(IEnumerable<SourceEntity> entities)
        {
            var entityEnumerable = entities.GetEnumerator();
            if (entityEnumerable.MoveNext() is false)
                return null;
            SourceEntity result = entityEnumerable.Current;

            switch (result.Type)
            {
                case EntityTypes.Function: break;
                default:
                    throw new NotImplementedException($"Cannot combine entities of type {result.Type}");
            }

            while (entityEnumerable.MoveNext())
            {
                var nextResult = entityEnumerable.Current;

                result.BriefDescription += nextResult.BriefDescription;
                result.ReturnDescription += nextResult.ReturnDescription;
                result.ThrowDescription += nextResult.ThrowDescription;
                result.Declarations = [.. result.Declarations, .. nextResult.Declarations];
                foreach (var x in nextResult.ParameterDescriptions)
                    if (result.ParameterDescriptions.ContainsKey(x.Key) is false)
                        result.ParameterDescriptions[x.Key] = x.Value;
            }

            return result;
        }
    }
}
