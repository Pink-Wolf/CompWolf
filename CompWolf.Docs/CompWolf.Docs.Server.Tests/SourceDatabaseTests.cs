using CompWolf.Docs.Server.Data;
using System.Xml.Linq;

namespace CompWolf.Docs.Server.Tests
{
    public class SourceDatabaseTests
    {
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
            var actual = () => SourceDatabase.GetEndOfBracketIndex('{', '}', text, startIndex);
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
            var actual = SourceDatabase.Namespace.SplitText(text)
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
            "comment", "type name;", "",
            TestName = "{m}(Variable: Full text is encompassed in brackets)")]
        [TestCase("{/* \r type name; \r */type name;}",
            "type name;", "type name;", "",
            TestName = "{m}(Variable: Ignore variable in comment)")]

        [TestCase("/*comment*/type name;",
            "comment", "type name;", "",
            TestName = "{m}(Variable)")]
        [TestCase("/*  comment  */  type  name  ;  ",
            "comment", "type  name  ;", "",
            TestName = "{m}(Variable: Whitespace)")]
        [TestCase("type name;",
            "", "type name;", "",
            TestName = "{m}(Variable: No comment)")]

        [TestCase("/*comment*/class test{body}",
            "comment", "class test", "{body",
            TestName = "{m}(Class)")]
        [TestCase("  /*  comment  */  class  test  {  body  }  ",
            "comment", "class  test", "{  body",
            TestName = "{m}(Class: Whitespace)")]
        [TestCase("class test{body}",
            "", "class test", "{body",
            TestName = "{m}(Class: No comment)")]

        [TestCase("/*comment*/#define test body\r",
            "comment", "#define test", "body",
            TestName = "{m}(Macro)")]
        [TestCase("  /*  comment  */  #define  test  body  \r",
            "comment", "#define  test", "body",
            TestName = "{m}(Macro: Whitespace)")]
        [TestCase("#define test body\r",
            "", "#define test", "body",
            TestName = "{m}(Macro: No comment)")]

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
            "comment", "using name", "=body;",
            TestName = "{m}(Alias)")]
        [TestCase("  /*  comment  */  using  name  =  body  ;  ",
            "comment", "using  name", "=  body  ;",
            TestName = "{m}(Alias: Whitespace)")]
        [TestCase("using name=body;",
            "", "using name", "=body;",
            TestName = "{m}(Alias: No comment)")]
        public void MemberEntity_SplitText(string text, params string[] expectedRaw)
        {
            var actual = SourceDatabase.MemberEntity.SplitText(text)
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
            "", "", "", "",
            TestName = "{m}(Empty)")]
        [TestCase("           ",
            "", "", "", "",
            TestName = "{m}(Whitespace)")]

        [TestCase("public:",
            "", "", "", "",
            TestName = "{m}(Empty visibility group)")]
        [TestCase("public://groupName\r",
            "", "", "", "",
            "groupName", "", "", "",
            TestName = "{m}(Empty group)")]
        [TestCase("public://groupName",
            "", "", "", "",
            "groupName", "", "", "",
            TestName = "{m}(Empty group without c++-style end of name)")]

        [TestCase("test",
            "", "test", "", "",
            TestName = "{m}(Default Group)")]
        [TestCase("testPublic protected:testProtected private:testPrivate",
            "", "testPublic", "testProtected", "testPrivate",
            TestName = "{m}(Default Group: Multiple visibilities)")]
        [TestCase("testPublic1 public:testPublic2 protected:testProtected public:testPublic3",
            "", "testPublic1 testPublic2 testPublic3", "testProtected", "",
            TestName = "{m}(Default Group: Multiple of same visibilities)")]
        [TestCase("testPublic protected:private:testPrivate",
            "", "testPublic", "", "testPrivate",
            TestName = "{m}(Default Group: Empty visibility)")]
        [TestCase("testPublic        protected:         testProtected",
            "", "testPublic", "testProtected", "",
            TestName = "{m}(Default Group: Whitespace)")]

        [TestCase("public://group1\rpublic1",
            "", "", "", "",
            "group1", "public1", "", "",
            TestName = "{m}(C++-style Named Group: public)")]
        [TestCase("protected://group1\rprotected1",
            "", "", "", "",
            "group1", "", "protected1", "",
            TestName = "{m}(C++-style Named Group: protected)")]
        [TestCase("  public  :  //  group1  \r  public1",
            "", "", "", "",
            "group1", "public1", "", "",
            TestName = "{m}(C++-style Named Group: whitespace)")]
        [TestCase("public://group1\rpublic1 protected:protected1",
            "", "", "", "",
            "group1", "public1", "protected1", "",
            TestName = "{m}(C++-style Named Group: Multiple visibilities)")]
        [TestCase("public:/*group1*/public1",
            "", "", "", "",
            "group1", "public1", "", "",
            TestName = "{m}(C-style Named Group: public)")]
        [TestCase("  public  :  /*  group1  */  public1",
            "", "", "", "",
            "group1", "public1", "", "",
            TestName = "{m}(C-style Named Group: whitespace)")]

        [TestCase("testPublic /* protected: */testProtected",
            "", "testPublic /* protected: */testProtected", "", "",
            TestName = "{m}(Ignore visibility within C-style comment)")]
        [TestCase("testPublic // protected: \rtestProtected",
            "", "testPublic // protected: \rtestProtected", "", "",
            TestName = "{m}(Ignore visibility within C++-style comment)")]
        [TestCase("testPublic \" protected: \"testProtected",
            "", "testPublic \" protected: \"testProtected", "", "",
            TestName = "{m}(Ignore visibility within comment)")]

        [TestCase("testPublic /* public://groupName */groupBody",
            "", "testPublic /* public://groupName */groupBody", "", "",
            TestName = "{m}(Ignore group within C-style comment)")]
        [TestCase("testPublic // public://groupName \rgroupBody",
            "", "testPublic // public://groupName \rgroupBody", "", "",
            TestName = "{m}(Ignore group within C++-style comment)")]
        [TestCase("testPublic \" public://groupName \"groupBody",
            "", "testPublic \" public://groupName \"groupBody", "", "",
            TestName = "{m}(Ignore group within comment)")]
        public void MemberGroup_SplitText(string text, params string[] expectedRaw)
        {
            var actual = SourceDatabase.MemberGroup.SplitText(text, true)
                .Select(x => new SourceDatabase.MemberGroup()
                {
                    GroupName = x.GroupName,
                    TextPerVisibility = x.TextPerVisibility.Select(x => (x.Key, x.Value.Trim())).ToDictionary()
                })
                .ToHashSet();
            var expected = new HashSet<SourceDatabase.MemberGroup>();
            for (int i = 0; i < expectedRaw.Length; i += 4)
            {
                expected.Add(new()
                {
                    GroupName = expectedRaw[i],
                    TextPerVisibility = new (string, string)[] {
                        ("public", expectedRaw[i + 1]),
                        ("protected", expectedRaw[i + 2]),
                        ("private", expectedRaw[i + 3])
                    }.ToDictionary(),
                });
            }

            CollectionAssert.AreEquivalent(expected, actual);
        }
    }
}