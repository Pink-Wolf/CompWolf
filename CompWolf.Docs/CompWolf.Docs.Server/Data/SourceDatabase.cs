﻿using CompWolf.Docs.Server.Models;
using System.Collections.Concurrent;
using System.Text.RegularExpressions;

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

            var fileData = string.Join(Newline, await File.ReadAllLinesAsync(filePath));
            var data = Namespace.SplitText(fileData)
                .SelectMany(GetEntitiesFromNamespace)
                .ToArray();

            _entityCache[filePath] = (data, fileTime);

            return data;
        }
        public static IEnumerable<SourceEntity> GetEntitiesFromNamespace(Namespace space)
        {
            var namespaceName = space.Name;
            var text = space.Text;

            foreach (var entityText in MemberEntity.SplitText(text))
            {
                var entityComment = entityText.Comment.Trim();
                var entityDeclaration = entityText.Declaration.Trim();
                var entityBody = entityText.Body.Trim();

                var templateMatch = Regex.Match(entityDeclaration, @"^\s*template\s*<");
                var templateEndIndex = templateMatch.Success
                    ? GetEndOfBracketIndexInCode('<', '>', entityDeclaration, templateMatch.Index + templateMatch.Length - 1)
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
                    .Select(x => x.EndsWith(';') ? x[..(x.Length - 1)] : x)
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
                    EntityTypes.Enum => cleanedDeclarationWords.Last(),
                    _ => throw new ArgumentOutOfRangeException(nameof(space),
                        "Tried deserializing entity of unknown type")
                };
                entityName = entityName.Trim();

                List<SourceEnumMember> enumElements = [];
                if (entityType == EntityTypes.Enum)
                {
                    enumElements = entityBody[1..].Split(',')
                        .Select(x =>
                        {
                            x = x.Trim();
                            var comment = "";
                            if (x.StartsWith(@"/*"))
                            {
                                var i = x.IndexOf(@"*/");
                                comment = x[2..i];
                                x = x[(i + 2)..];

                                char[] trimCharacters = (comment[0] == '*')
                                    ? [' ', '*']
                                    : [' '];
                                comment = string.Join(Newline, comment.Split(Newline).Select(x => x.Trim(trimCharacters)));
                            }
                            else if (x.StartsWith(@"//"))
                            {
                                var i = x.IndexOf(Newline);
                                comment = x[2..i];
                                x = x[(i + Newline.Length)..];
                            }
                            var name = x[..Regex.Match(x, @"^\s*(\S+)").Groups[1].Length];
                            return new SourceEnumMember()
                            {
                                Description = comment.Trim(),
                                Name = name,
                            };
                        })
                        .ToList();
                }

                if (entityType == EntityTypes.Class)
                {
                    nestedEntities = MemberGroup.SplitText(entityBody, isStruct)
                        .Select(x => (x.GroupName.Trim(), x.TextPerVisibility
                                .Where(x => x.Item1 != "private")
                                .SelectMany(x => GetEntitiesFromNamespace(new()
                                {
                                    Name = namespaceName,
                                    Text = x.Item2,
                                })
                                    .Select(Value =>
                                    {
                                        if (x.Item1 == "protected")
                                            foreach (var declaration in Value.Declarations)
                                                declaration.Protected = true;
                                        return Value;
                                    }))
                                .GroupBy(x => x.Name)
                                .Select(entityEnumerator =>
                                {
                                    var entityGroup = entityEnumerator.ToList();
                                    var entity = entityGroup[0];

                                    if (entityGroup.Count > 1)
                                    {
                                        for (int i = 1; string.IsNullOrEmpty(entity.BriefDescription) && i < entityGroup.Count; ++i)
                                        {
                                            entity.BriefDescription = entityGroup[i].BriefDescription;
                                        }

                                        entity.Declarations = entityGroup.SelectMany(x => x.Declarations).ToArray();
                                    }

                                    return entity;
                                })
                                .ToArray()
                            ))
                            .Where(x => x.Item2.Length > 0)
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
                    if (entityComment.Length > 0)
                    {
                        var commentTarget = mainCommentLines;
                        bool javaDocComment = entityComment[0] == '*';
                        foreach (var rawLine in entityComment.Split(Newline))
                        {
                            var newCommentText = javaDocComment
                                ? rawLine.TrimStart(' ', '*')
                                : rawLine.TrimStart(' ');
                            newCommentText = newCommentText.TrimEnd();

                            var sectionMatch = Regex.Match(newCommentText, @"^@(\w+)\s");
                            if (sectionMatch.Success)
                            {
                                newCommentText = newCommentText[sectionMatch.Length..].TrimStart();

                                switch (sectionMatch.Groups[1].Value)
                                {
                                    case "param":
                                        var splitCommentText = newCommentText.Split(' ', 2);
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

                            commentTarget.Add(newCommentText.Trim());
                        }
                    }

                    mainComment = string.Join(Newline, mainCommentLines).Trim();
                    briefComment = string.Join(Newline, briefCommentLines).Trim();
                    returnComment = string.Join(Newline, returnCommentLines).Trim();
                    throwComment = string.Join(Newline, throwCommentLines).Trim();
                    parameterComments = parameterCommentLines.Select(x => (x.Key.Trim(), string.Join(Newline, x.Value).Trim()))
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
                        BriefDescription = briefComment,
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
                        EnumMembers = [.. enumElements],
                    };
                }
            }
        }

        public class MemberEntity
        {
            public string Comment { get; set; } = "";
            public string Declaration { get; set; } = "";
            public string Body { get; set; } = "";

            public override string ToString()
                => $"\r{Comment}\r{Declaration}\r{Body}\r";
            public override bool Equals(object? obj)
                => obj is MemberEntity other
                && Comment.Equals(other.Comment)
                && Declaration.Equals(other.Declaration)
                && Body.Equals(other.Body)
                ;
            public override int GetHashCode()
                => HashCode.Combine(Comment, Declaration, Body);

            public static IEnumerable<MemberEntity> SplitText(string text)
            {
                if (string.IsNullOrEmpty(text)) yield break;

                for (int declarationIndex = (text[0] == '{') ? 1 : 0; declarationIndex < text.Length; ++declarationIndex)
                {
                    {
                        var i = SkipNonRelevantCharInCode(text, declarationIndex);
                        if (i >= 0)
                        {
                            declarationIndex = i - 1;
                            continue;
                        }
                    }

                    int bodyIndex, endIndex;
                    {
                        var firstChar = text[declarationIndex];

                        if (char.IsWhiteSpace(firstChar)) continue;
                        if (firstChar == ';') continue;
                        if (firstChar == '{')
                        {
                            declarationIndex = GetEndOfBracketIndexInCode('{', '}', text, firstChar);
                            --declarationIndex;
                            continue;
                        }
                        if (firstChar == '}')
                        {
                            break;
                        }
                        if (firstChar == '(')
                        {
                            declarationIndex = GetEndOfBracketIndexInCode('(', ')', text, firstChar);
                            --declarationIndex;
                            continue;
                        }
                        if (firstChar == '/')
                        {
                            if (declarationIndex + 2 < text.Length)
                                switch (text[declarationIndex + 1])
                                {
                                    case '/':
                                        declarationIndex = text.IndexOf(Newline, declarationIndex + 2);
                                        --declarationIndex;
                                        continue;
                                    case '*':
                                        declarationIndex = text.IndexOf("*/", declarationIndex + 2) + 2;
                                        --declarationIndex;
                                        continue;
                                    default: break;
                                }
                        }

                        bodyIndex = declarationIndex;
                        var macroMatch = Regex.Match(text[bodyIndex..], @"^\s*#define\s+\S+\s*");
                        if (macroMatch.Success)
                        {
                            bodyIndex += macroMatch.Length;

                            if (text[bodyIndex] == '(')
                            {
                                bodyIndex = GetEndOfBracketIndexInCode('(', ')', text, bodyIndex) + 1;
                            }

                            endIndex = text.IndexOf(Newline, bodyIndex);
                            if (endIndex < 0) endIndex = text.Length;
                        }
                        else
                        {
                            for (; bodyIndex < text.Length; ++bodyIndex)
                            {
                                switch (text[bodyIndex])
                                {
                                    case ';': break;
                                    case '{': break;
                                    case '=':
                                        if (Regex.Match(text[..bodyIndex], @"\soperator\s*$").Success)
                                            continue; // "=" in method name "operator=" is not the end
                                        else
                                            break;
                                    case '<':
                                        bodyIndex = GetEndOfBracketIndexInCode('<', '>', text, bodyIndex);
                                        --bodyIndex;
                                        continue;
                                    case '(':
                                        bodyIndex = GetEndOfBracketIndexInCode('(', ')', text, bodyIndex);
                                        --bodyIndex;
                                        continue;
                                    default: continue;
                                }
                                break;
                            }

                            endIndex = bodyIndex;
                            switch (text[endIndex])
                            {
                                case ';':
                                    break;
                                case '{':
                                    endIndex = GetEndOfBracketIndexInCode('{', '}', text, endIndex);
                                    break;
                                case '=':
                                    endIndex = text.IndexOf(';', bodyIndex + 1);
                                    // Concept may be for example "= requires(T t) {}"
                                    var potentialEntityType = text.IndexOf('{', bodyIndex + 1, endIndex - (bodyIndex + 1));
                                    if (potentialEntityType > 0)
                                    {
                                        endIndex = potentialEntityType;
                                        goto case '{';
                                    }
                                    break;
                                default:
                                    throw new FormatException($"Cannot find end of entity {((text.Length > declarationIndex + 32)
                                        ? text[declarationIndex..(declarationIndex + 32)] + "..."
                                        : text[declarationIndex..]
                                    )}");
                            }
                        }
                    }

                    {
                        MemberEntity member = new()
                        {
                            Comment = "",
                            Declaration = text[declarationIndex..bodyIndex],
                            Body = endIndex > bodyIndex ? text[bodyIndex..endIndex] : "",
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
                                member.Comment = text[(commentIndex + 2)..(previousWordEndIndex - 1)];
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
            public List<(string, string)> TextPerVisibility { get; set; } = [];

            public override string ToString()
                => $"{GroupName}: {string.Join("  ", TextPerVisibility.Select((key, value) => $"{key}: {value}"))}";
            public override bool Equals(object? obj)
                => obj is MemberGroup other
                && GroupName.Equals(other.GroupName)
                && TextPerVisibility.Join(other.TextPerVisibility, x => x, x => x,
                    (lhs, rhs) => lhs.Equals(rhs))
                    .Contains(false) is false;
            public override int GetHashCode()
                => ToString().GetHashCode();

            public static IEnumerable<MemberGroup> SplitText(string text, bool publicByDefault)
            {
                if (text.Length == 0) text = " ";
                if (text[0] == '{') text = text[1..];
                text = ' ' + text + ' ';

                Dictionary<string, List<(string, string)>> groups = [];
                void AddToGroup(string groupName, string visibility, string text)
                {
                    if (groups.TryGetValue(groupName, out var groupContent) is false)
                    {
                        groups[groupName] = groupContent = [];
                    }
                    groupContent.Add((visibility, text));
                }

                {
                    var handledCharCount = 0;
                    var currentGroup = "";
                    var currentVisibility = publicByDefault ? "public" : "private";

                    string? currentMaybeVisibility = null;
                    var currentMaybeEndIndex = 0;

                    for (var textIndex = handledCharCount; textIndex < text.Length; ++textIndex)
                    {
                        var newIndex = SkipNonRelevantCharInCode(text, textIndex);
                        if (newIndex > textIndex)
                        {
                            textIndex = --newIndex;
                            continue;
                        }

                        if (char.IsWhiteSpace(text[textIndex])) continue;
                        if (text[textIndex] == '{')
                        {
                            textIndex = GetEndOfBracketIndexInCode('{', '}', text, textIndex);
                        }
                        if (char.IsWhiteSpace(text[textIndex - 1]) is false) switch (text[textIndex - 1])
                            {
                                case ';': break;
                                case '}': break;
                                default: continue;
                            }

                        var currentText = text[textIndex..];

                        var newVisibilityStart = -1;
                        if (currentMaybeVisibility != null && currentText[0] == ':')
                        {
                            newVisibilityStart = textIndex + 1;
                        }
                        else
                        {
                            currentMaybeVisibility
                                = currentText.StartsWith("public") ? "public"
                                : currentText.StartsWith("protected") ? "protected"
                                : currentText.StartsWith("private") ? "private"
                                : null;

                            if (currentMaybeVisibility != null)
                            {
                                currentMaybeEndIndex = textIndex;

                                if (currentText.Length > currentMaybeVisibility.Length)
                                {
                                    var c = currentText[currentMaybeVisibility.Length];
                                    if (c == ':')
                                    {
                                        newVisibilityStart = textIndex + currentMaybeVisibility.Length + 1;
                                    }
                                    else if (char.IsWhiteSpace(c) is false)
                                        currentMaybeVisibility = null;
                                }
                            }
                        }

                        if (newVisibilityStart > -1)
                        {
                            AddToGroup(currentGroup, currentVisibility, text[handledCharCount..currentMaybeEndIndex]);
                            textIndex = newVisibilityStart;

                            currentVisibility = currentMaybeVisibility!;
                            currentMaybeVisibility = null;

                            var newGroupMatch = Regex.Match(text[newVisibilityStart..], @"^\s*\/\*(.*?)\*\/");
                            if (newGroupMatch.Success is false)
                                newGroupMatch = Regex.Match(text[newVisibilityStart..], @"^\s*\/\/(.*?)(?:$|" + Newline + ')');
                            if (newGroupMatch.Success)
                            {
                                currentGroup = newGroupMatch.Groups[1].Value.Trim();
                                textIndex += newGroupMatch.Length;
                            }
                            handledCharCount = textIndex;
                        }
                    }
                    AddToGroup(currentGroup, currentVisibility, text[handledCharCount..]);
                }

                return groups
                    .Select(group => new MemberGroup()
                    {
                        GroupName = group.Key,
                        TextPerVisibility = group.Value,
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
            public override int GetHashCode()
                => HashCode.Combine(Name, Text);

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

        /// <summary> Returns the index after skipping non-relevant code at the given index of the given text.
        /// Returns -1 if the index points to relevant code. </summary>
        public static int SkipNonRelevantCharInCode(string text, int index)
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
                    return index;
                case '/':
                    switch (text[++index])
                    {
                        case '/':
                            {
                                index = text.IndexOf(Newline, ++index);
                                if (index < 0)
                                    return text.Length;
                                index += Newline.Length;
                                return index;
                            }
                        case '*':
                            {
                                index = text.IndexOf("*/", ++index);
                                if (index < 0)
                                    throw new FormatException($"Could not find */ after index {oldIndex} of {text}");
                                index += 2;
                                return index;
                            }
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            return -1;
        }

        public static IEnumerable<int> ForeachRelevantCharInCode(string text, int startIndex = 0)
        {
            for (var index = startIndex; index < text.Length; ++index)
            {
                var newIndex = SkipNonRelevantCharInCode(text, index);
                if (newIndex >= 0)
                {
                    index = --newIndex;
                    continue;
                }
                yield return index;
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
                    throw new NotImplementedException($"Cannot combine entities of type {result.Type}. The entities were:\n"
                        + string.Join('\n', entities));
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
