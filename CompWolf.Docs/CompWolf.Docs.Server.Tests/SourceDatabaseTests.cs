using CompWolf.Docs.Server.Data;
using CompWolf.Docs.Server.Models;
using System.Text.Json;

namespace CompWolf.Docs.Server.Tests
{
    public class SourceDatabaseTests
    {
        private readonly static JsonSerializerOptions serializerOptions = new()
        {
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        };

        [SetUp]
        public void Setup()
        {
            SourceDatabase.Newline = "\r";
        }

        [TestCase(3, "{  }", 0, TestName = "{m}(Basic)")]
        [TestCase(3, "{  }  ", 0, TestName = "{m}(Whitespace)")]
        [TestCase(3 - 2, "{  }", -2, TestName = "{m}(Negative index)")]
        [TestCase(5, "{ {} }", 0, TestName = "{m}(Nested)")]
        [TestCase(10, "{{ {} {} }}", 0, TestName = "{m}(Multiple nested)")]
        [TestCase(9, "{{ {} {} }", 1, TestName = "{m}(Positive index)")]
        public void GetEndOfBracketIndex(int expected, string text, int startIndex)
        {
            var actual = SourceDatabase.GetEndOfBracketIndex('{', '}', text, startIndex);
            Assert.That(actual, Is.EqualTo(expected));
        }
        [TestCase(" { }", 0, TestName = "{m}(No start-bracket at start)")]
        [TestCase("{  }", 1, TestName = "{m}(No start-bracket at start index)")]
        [TestCase("{  ", 0, TestName = "{m}(No end)")]
        [TestCase("{{  }  ", 0, TestName = "{m}(No end with nested)")]
        [TestCase("{  }", 10, TestName = "{m}(Index past end)")]
        public void GetEndOfBracketIndex_Throws(string text, int startIndex)
        {
            int actual() => SourceDatabase.GetEndOfBracketIndex('{', '}', text, startIndex);
            Assert.That(actual, Throws.Exception);
        }
        [TestCase(7, @"{....__}__ }", 0, TestName = "{m}(Basic)")]
        [TestCase(7, @"{....""""}__ }", 0, TestName = "{m}(String right before end)")]
        [TestCase(7, @"{...""""_}__ }", 0, TestName = "{m}(String before end)")]
        [TestCase(11, @"{...._""}""_ }", 0, TestName = "{m}(Ignore what is in string)")]
        [TestCase(11, @"{...""\""}""_ }", 0, TestName = "{m}(Differentiate \" and \\\")")]
        [TestCase(16, @"{R""test(""})test""}", 0, TestName = "{m}(Raw string literal)")]
        [TestCase(7, @"{../**/}__ }", 0, TestName = "{m}(C-style comment before end)")]
        [TestCase(11, @"{..../*}*/ }", 0, TestName = "{m}(Ignore what is in C-style comment)")]
        [TestCase(7, "{...//\r}__ }", 0, TestName = "{m}(C++-style comment before end)")]
        [TestCase(11, "{....//}\r_ }", 0, TestName = "{m}(Ignore what is in C++-style comment)")]
        [TestCase(11, @"{.../*/}*/ }", 0, TestName = "{m}(/*/ should not be read as /**/)")]
        public void GetEndOfBracketIndexInCode(int expected, string text, int startIndex)
        {
            var actual = SourceDatabase.GetEndOfBracketIndexInCode('{', '}', text, startIndex);
            Assert.That(actual, Is.EqualTo(expected));
        }

        [TestCase("", TestName = "{m}(Empty)")]
        [TestCase("         ", TestName = "{m}(Whitespace)")]
        [TestCase("namespace name{body}",
            "name", "body",
            TestName = "{m}(Basic)")]
        [TestCase("   namespace name{body}",
            "name", "body",
            TestName = "{m}(Basic with whitespace)")]
        [TestCase("   namespace name{  body  }",
            "name", "body",
            TestName = "{m}(Basic with more whitespace)")]
        [TestCase("   namespace   name  {body}  ",
            "name", "body",
            TestName = "{m}(Basic with even more whitespace)")]

        [TestCase("ignoreThis namespace name{body} ignoreThat",
            "name", "body",
            TestName = "{m}(Ignore non-namespace stuff)")]

        [TestCase("namespace name::nestedName{body}",
            "name::nestedName", "body",
            TestName = "{m}(Nested namespace definition)")]
        [TestCase("namespace name{test namespace nestedName {body}}",
            "name", "test",
            "name::nestedName", "body",
            TestName = "{m}(Nested namespace)")]

        [TestCase(@"""namespace name{body}""", TestName = "{m}(Ignore what is in string)")]
        [TestCase(@""" ""namespace name{body}",
            "name", "body",
            TestName = "{m}(string before namespace)")]

        [TestCase(@"//namespace name{body}", TestName = "{m}(Ignore C++-style comment)")]
        [TestCase("//\r namespace name{body}",
            "name", "body",
            TestName = "{m}(C++-style comment before namespace)")]
        [TestCase(@"/*namespace name{body}*/", TestName = "{m}(Ignore C-style comment)")]
        [TestCase(@"/* */ namespace name{body}",
            "name", "body",
            TestName = "{m}(C-style comment before namespace)")]
        public void Namespace_SplitText(string text, params string[] expectedRaw)
        {
            var actual = SourceDatabase.Namespace.SplitText(text, $"{nameof(SourceDatabaseTests)}.cs")
                .Select(x => new SourceDatabase.Namespace()
                {
                    Name = x.Name,
                    Text = x.Text.Trim(),
                })
                .ToHashSet();
            var expected = new HashSet<SourceDatabase.Namespace>();
            for (int i = 0; i < expectedRaw.Length; i += 2)
                expected.Add(new()
                {
                    Name = expectedRaw[i],
                    Text = expectedRaw[i + 1],
                });

            CollectionAssert.AreEquivalent(expected, actual);
        }

        [TestCase("", TestName = "{m}(Empty)")]
        [TestCase("         ", TestName = "{m}(Whitespace)")]
        [TestCase("{/*comment*/type name;}",
            "comment", "type name", "",
            TestName = "{m}(Variable: Full text is encompassed in brackets)")]
        [TestCase("{/* \r type name; \r */type name;}",
            "type name;", "type name", "",
            TestName = "{m}(Variable: Ignore variable in comment)")]

        [TestCase("/*comment*/type name;",
            "comment", "type name", "",
            TestName = "{m}(Variable)")]
        [TestCase("/*  comment  */  type  name  ;  ",
            "comment", "type  name", "",
            TestName = "{m}(Variable: Whitespace)")]
        [TestCase("type name;",
            "", "type name", "",
            TestName = "{m}(Variable: No comment)")]

        [TestCase("/*comment*/class test{body};",
            "comment", "class test", "{body",
            TestName = "{m}(Class)")]
        [TestCase("  /*  comment  */  class  test  {  body  }  ;  ",
            "comment", "class  test", "{  body",
            TestName = "{m}(Class: Whitespace)")]
        [TestCase("class test{body};",
            "", "class test", "{body",
            TestName = "{m}(Class: No comment)")]
        [TestCase("/*comment*/class test1{body1};/*comment*/class test2{body2};",
            "comment", "class test1", "{body1",
            "comment", "class test2", "{body2",
            TestName = "{m}(Class: Multiple)")]

        [TestCase("/*comment*/#define test body\r",
            "comment", "#define test", "body",
            TestName = "{m}(Macro)")]
        [TestCase("  /*  comment  */  #define  test  body  \r",
            "comment", "#define  test", "body",
            TestName = "{m}(Macro: Whitespace)")]
        [TestCase("#define test body\r",
            "", "#define test", "body",
            TestName = "{m}(Macro: No comment)")]
        [TestCase("/*comment*/#define test ;int test();\r",
            "comment", "#define test", ";int test();",
            TestName = "{m}(Macro: Function in macro)")]

        [TestCase("/*comment*/#define test() body\r",
            "comment", "#define test()", "body",
            TestName = "{m}(Parameter-macro)")]
        [TestCase("  /*  comment  */  #define  test  (  )  body  \r",
            "comment", "#define  test  (  )", "body",
            TestName = "{m}(Parameter-macro: Whitespace)")]
        [TestCase("#define test() body\r",
            "", "#define test()", "body",
            TestName = "{m}(Parameter-macro: No comment)")]
        [TestCase("/*comment*/#define test() body",
            "comment", "#define test()", "body",
            TestName = "{m}(Parameter-macro: end instead of newline)")]

        [TestCase("/*comment*/using name=body;",
            "comment", "using name", "=body",
            TestName = "{m}(Alias)")]
        [TestCase("  /*  comment  */  using  name  =  body  ;  ",
            "comment", "using  name", "=  body",
            TestName = "{m}(Alias: Whitespace)")]
        [TestCase("using name=body;",
            "", "using name", "=body",
            TestName = "{m}(Alias: No comment)")]
        public void MemberEntity_SplitText(string text, params string[] expectedRaw)
        {
            var actual = SourceDatabase.MemberEntity.SplitText(text, $"{nameof(SourceDatabaseTests)}.cs")
                .Select(x => new SourceDatabase.MemberEntity()
                {
                    Comment = x.Comment.Trim(),
                    Declaration = x.Declaration.Trim(),
                    Body = x.Body.Trim(),
                })
                .ToHashSet();
            var expected = new HashSet<SourceDatabase.MemberEntity>();
            for (int i = 0; i < expectedRaw.Length; i += 3)
            {
                expected.Add(new()
                {
                    Comment = expectedRaw[i + 0],
                    Declaration = expectedRaw[i + 1],
                    Body = expectedRaw[i + 2],
                });
            }

            CollectionAssert.AreEquivalent(expected, actual);
        }

        [TestCase("",
            "", "0",
            TestName = "{m}(Empty)")]
        [TestCase("           ",
            "", "0",
            TestName = "{m}(Whitespace)")]

        [TestCase("public:",
            "", "0",
            TestName = "{m}(Empty visibility group)")]
        [TestCase("public://groupName\r",
            "", "0",
            "groupName", "0",
            TestName = "{m}(Empty group)")]
        [TestCase("public://groupName",
            "", "0",
            "groupName", "0",
            TestName = "{m}(Empty group without c++-style end of name)")]

        [TestCase("test",
            "", "1", "public", "test",
            TestName = "{m}(Default Group)")]
        [TestCase("testPublic protected:testProtected private:testPrivate",
            "", "3", "public", "testPublic", "protected", "testProtected", "private", "testPrivate",
            TestName = "{m}(Default Group: Multiple visibilities)")]
        [TestCase("testPublic1 public:testPublic2 protected:testProtected public:testPublic3",
            "", "2", "public", "testPublic1 testPublic2 testPublic3", "protected", "testProtected",
            TestName = "{m}(Default Group: Multiple of same visibilities)")]
        [TestCase("testPublic protected:private:testPrivate",
            "", "2", "public", "testPublic", "private", "testPrivate",
            TestName = "{m}(Default Group: Empty visibility)")]
        [TestCase("testPublic        protected:         testProtected",
            "", "2", "public", "testPublic", "protected", "testProtected",
            TestName = "{m}(Default Group: Whitespace)")]

        [TestCase("public://group1\rpublic1",
            "", "0",
            "group1", "1", "public", "public1",
            TestName = "{m}(C++-style Named Group: public)")]
        [TestCase("protected://group1\rprotected1",
            "", "0",
            "group1", "1", "protected", "protected1",
            TestName = "{m}(C++-style Named Group: protected)")]
        [TestCase("  public  :  //  group1  \r  public1",
            "", "0",
            "group1", "1", "public", "public1",
            TestName = "{m}(C++-style Named Group: whitespace)")]
        [TestCase("public://group1\rpublic1 protected:protected1",
            "", "0",
            "group1", "2", "public", "public1", "protected", "protected1",
            TestName = "{m}(C++-style Named Group: Multiple visibilities)")]
        [TestCase("public:/*group1*/public1",
            "", "0",
            "group1", "1", "public", "public1",
            TestName = "{m}(C-style Named Group: public)")]
        [TestCase("  public  :  /*  group1  */  public1",
            "", "0",
            "group1", "1", "public", "public1",
            TestName = "{m}(C-style Named Group: whitespace)")]

        [TestCase("testPublic /* protected: */testProtected",
            "", "1", "public", "testPublic /* protected: */testProtected",
            TestName = "{m}(Ignore visibility within C-style comment)")]
        [TestCase("testPublic // protected: \rtestProtected",
            "", "1", "public", "testPublic // protected: \rtestProtected",
            TestName = "{m}(Ignore visibility within C++-style comment)")]
        [TestCase("testPublic \" protected: \"testProtected",
            "", "1", "public", "testPublic \" protected: \"testProtected",
            TestName = "{m}(Ignore visibility within comment)")]

        [TestCase("testPublic /* public://groupName */groupBody",
            "", "1", "public", "testPublic /* public://groupName */groupBody",
            TestName = "{m}(Ignore group within C-style comment)")]
        [TestCase("testPublic // public://groupName \rgroupBody",
            "", "1", "public", "testPublic // public://groupName \rgroupBody",
            TestName = "{m}(Ignore group within C++-style comment)")]
        [TestCase("testPublic \" public://groupName \"groupBody",
            "", "1", "public", "testPublic \" public://groupName \"groupBody",
            TestName = "{m}(Ignore group within comment)")]
        public void MemberGroup_SplitText(string text, params string[] expectedRaw)
        {
            var actual = SourceDatabase.MemberGroup.SplitText(text, true, $"{nameof(SourceDatabaseTests)}.cs")
                .Select(x => new SourceDatabase.MemberGroup()
                {
                    GroupName = x.GroupName,
                    TextPerVisibility = x.TextPerVisibility.Select(x => (x.Item1, x.Item2.Trim())).ToList()
                })
                .ToHashSet();
            var expected = new HashSet<SourceDatabase.MemberGroup>();
            for (int i = 0; i < expectedRaw.Length;)
            {
                SourceDatabase.MemberGroup newGroup = new()
                {
                    GroupName = expectedRaw[i],
                    TextPerVisibility = [],
                };
                ++i;
                int endIndex = int.Parse(expectedRaw[i]) * 2 + i + 1;
                for (++i; i < endIndex; i += 2)
                {
                    newGroup.TextPerVisibility.Add((expectedRaw[i], expectedRaw[i + 1]));
                }
                expected.Add(newGroup);
            }

            CollectionAssert.AreEquivalent(expected, actual);
        }

        [TestCase("GetEntitiesFromNamespace_ComplexTest.cpp", "GetEntitiesFromNamespace_ComplexTest.json",
            TestName = $"{nameof(GetEntitiesFromNamespace)}(Complex Test)")]
        public void GetEntitiesFromNamespace_FromFile(string actualFilepath, string expectedFilepath)
            => GetEntitiesFromNamespace(File.ReadAllText(actualFilepath).ReplaceLineEndings("\r"),
                File.ReadAllText(expectedFilepath));

        [TestCase("GetEntitiesFromNamespace_ComplexTest_NoWhitespace.cpp",
            "GetEntitiesFromNamespace_ComplexTest_NoWhitespace.json",
            TestName = $"{nameof(GetEntitiesFromNamespace)}(Complex Test: No whitespace)")]
        public void GetEntitiesFromNamespace_FromFile_NoWhitespace(string actualFilepath, string expectedFilepath)
            => GetEntitiesFromNamespace(
                string.Join("", File.ReadAllText(actualFilepath)
                    .ReplaceLineEndings().Split(Environment.NewLine)
                    .Select(x => x.Trim()))
                    .Replace(@"|NEWLINE|", SourceDatabase.Newline),
                File.ReadAllText(expectedFilepath)
            );

        [TestCase("", "[]",
            TestName = "{m}(Empty)")]
        [TestCase("           ", "[]",
            TestName = "{m}(Whitespace)")]
        [TestCase("/**line1\r*line2*/void test();",
            @"[{
                ""name"": ""test"",
                ""type"":""function"",
                ""descriptions"": [""line1\rline2""],
                ""declarations"":[{
                    ""description"": """",
                    ""declaration"": ""void test();"",
                    ""protected"": false
                }]
            }]",
            TestName = "{m}(Ignore * at start of /**-comment's lines)")]
        [TestCase("/*line1\r*line2*/void test();",
            @"[{
                ""name"": ""test"",
                ""type"": ""function"",
                ""descriptions"": [""line1\r*line2""],
                ""declarations"": [{
                    ""description"": """",
                    ""declaration"": ""void test();"",
                    ""protected"": false
                }]
            }]",
            TestName = "{m}(Do not ignore * at start of non-/**-comment's lines)")]

        public void GetEntitiesFromNamespace(string namespaceBody, string expectedRaw)
        {
            var actual = SourceDatabase.GetEntitiesFromNamespace(new()
            {
                Name = "namespaceName",
                Text = namespaceBody,
            }, $"{nameof(SourceDatabaseTests)}.cs").ToHashSet();

            static IEnumerable<SourceEntity> FormatExpected(IEnumerable<SourceEntity> entities)
            {
                foreach (var entity in entities)
                {
                    entity.Namespace ??= "namespaceName";
                    entity.Descriptions ??= [];
                    entity.ReturnDescription ??= "";
                    entity.ParameterDescriptions ??= [];
                    entity.ThrowDescription ??= "";
                    entity.Members ??= [];
                    entity.Warnings ??= [];
                    entity.Related ??= [];
                    entity.BaseClasses ??= [];
                    entity.TemplateParameterDescriptions ??= [];
                    foreach (var members in entity.Members.Values)
                        FormatExpected(members);
                    entity.EnumMembers ??= [];
                }
                return entities;
            };

            var expected = new HashSet<SourceEntity>(JsonSerializer.Deserialize<SourceEntity[]>(expectedRaw, serializerOptions)!);
            FormatExpected(expected);

            CollectionAssert.AreEquivalent(expected, actual);
        }
    }
}