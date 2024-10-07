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
            parentMakeListData = parentMakeListData[..(parentMakeListData.IndexOf(')') - 1)];

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
            foreach (var (entityComment, entityDeclaration, entityBody) in SplitMemberGroupIntoMember(text))
            {
                var templateMatch = Regex.Match(entityDeclaration, @"\s*template\s*<");
                var templateEndIndex = templateMatch.Success
                    ? GetEndOfBracketIndex('<', '>', entityDeclaration, templateMatch.Index)
                    : -1;
                var noTemplateDeclaration = entityDeclaration[(templateEndIndex + 1)..];
                var declarationWords = noTemplateDeclaration
                    .Split(Array.Empty<char>(), StringSplitOptions.RemoveEmptyEntries)
                    .ToList();

                IEnumerable <SourceEntity> nestedEntities = [];

                bool IsType(string type) => declarationWords[0] == type;
                bool isStruct = IsType("struct");
                var entityType = isStruct ? EntityTypes.Class
                    : IsType("class") ? EntityTypes.Class
                    : IsType("concept") ? EntityTypes.Concept
                    : IsType("enum") ? EntityTypes.Enum
                    : IsType("using") ? EntityTypes.Alias
                    : IsType("#define") ? EntityTypes.Macro
                    : noTemplateDeclaration.Contains('(') ? EntityTypes.Function
                    : EntityTypes.Variable;

                if (entityType == EntityTypes.Class)
                {
                    nestedEntities = SplitIntoMemberGroups(entityBody, isStruct)
                        .SelectMany(x => GetEntities(x.Value, namespaceName));
                }

                var entityName = entityType switch
                {
                    EntityTypes.Class => declarationWords[1].StartsWith('[')
                        ? declarationWords[2]
                        : declarationWords[1],
                    EntityTypes.Function => Regex.Match(noTemplateDeclaration, @"(\S+)\s*\(").Groups[1].Value,
                    EntityTypes.Macro => declarationWords[1][0..declarationWords[1].IndexOf('(')],
                    EntityTypes.Alias => declarationWords[1],
                    EntityTypes.Concept => declarationWords[1],
                    EntityTypes.Variable => declarationWords[1],
                    _ => throw new ArgumentOutOfRangeException("Tried deserializing entity of unknown type")
                };

                List<string> mainComment = [];
                Dictionary<string, List<string>> parameterComment = [];
                List<string> returnComment = [];
                List<string> throwComment = [];
                {
                    var commentTarget = mainComment;
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
                                    commentTarget = parameterComment[splitCommentText[0]] = [];
                                    break;
                                case "return":
                                    commentTarget = returnComment;
                                    break;
                                case "throws":
                                    commentTarget = throwComment;
                                    break;
                                default:
                                    throw new FormatException($"Cannot deserialize Javadoc tag {sectionMatch.Groups[1]}");
                            }
                        }

                        commentTarget.Add(line.Trim());
                    }
                }

                yield return new()
                {
                    Name = entityName,
                    Type = entityType,
                    BriefDescription = string.Join(Environment.NewLine, mainComment),
                    Members = nestedEntities.ToArray(),
                };
            }
        }

        static IEnumerable<(string comment, string declaration, string body)> SplitMemberGroupIntoMember(string text)
        {
            Match commentMatch;
            while ((commentMatch = Regex.Match(text, @"\/\*(\*[\s\S]*?)\*\/")).Success)
            {
                var commentIndex = commentMatch.Index;
                var declarationIndex = commentIndex + commentMatch.Length;
                var bodyIndex = text.IndexOfAny([';', '{'], declarationIndex);
                var bodyEndIndex = text[bodyIndex] == ';'
                    ? bodyIndex
                    : GetEndOfBracketIndex('{', '}', text, bodyIndex);

                var comment = text[commentIndex..(declarationIndex - 1)];
                var declaration = text[bodyIndex] == ';'
                    ? text[declarationIndex..bodyIndex]
                    : text[declarationIndex..(bodyIndex - 1)]
                ;
                var body = text[bodyIndex] == ';'
                    ? ""
                    : text[bodyIndex..bodyEndIndex]
                ;

                yield return (comment, declaration, body);

                text = text[bodyEndIndex..];
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
                    string namespacePattern = @"namespace\s+([\w\:]+)\s*$";
                    var namespaceMatch = Regex.Match(text[0..i], namespacePattern);
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
                    var body = text[space.startIndex..space.paranthesisLevel];
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
                            groups[currentGroup] = string.Join(null, [groups[currentGroup], .. textSplit[i..newI]]);
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
    }
}
