using CompWolf.Docs.Server.Data;

namespace CompWolf.Docs.Server.Tests
{
    public class SourceDatabaseTests
    {
        [TestCase(3, "{  }", 0)]
        [TestCase(3, "{  }  ", 0)]
        [TestCase(3-2, "{  }", -2)]
        [TestCase(5, "{ {} }", 0)]
        [TestCase(10, "{{ {} {} }}", 0)]
        [TestCase(9, "{{ {} {} }", 1)]
        public void GetEndOfBracketIndex(int expected, string text, int startIndex)
        {
            var actual = SourceDatabase.GetEndOfBracketIndex('{', '}', text, startIndex);
            Assert.That(actual, Is.EqualTo(expected));
        }
        [TestCase(" { }", 0)]
        [TestCase("{  }", 1)]
        [TestCase("{  ", 0)]
        [TestCase("{{  }  ", 0)]
        [TestCase("{  }", 10)]
        public void GetEndOfBracketIndex_Throws(string text, int startIndex)
        {
            var actual = () => SourceDatabase.GetEndOfBracketIndex('{', '}', text, startIndex);
            Assert.That(actual, Throws.Exception);
        }
        [TestCase(7, @"{....__}__ }", 0)]
        [TestCase(7, @"{....""""}__ }", 0)]
        [TestCase(7, @"{...""""_}__ }", 0)]
        [TestCase(11, @"{...._""}""_ }", 0)]
        [TestCase(11, @"{...""\""}""_ }", 0)]
        [TestCase(16, @"{R""test(""})test""}", 0)]
        [TestCase(7, @"{../**/}__ }", 0)]
        [TestCase(11, @"{..../*}*/ }", 0)]
        [TestCase(7, "{...//\r}__ }", 0)]
        [TestCase(11, "{....//}\r_ }", 0)]
        [TestCase(11, @"{.../*/}*/ }", 0)]
        public void GetEndOfBracketIndexInCode(int expected, string text, int startIndex)
        {
            var actual = SourceDatabase.GetEndOfBracketIndexInCode('{', '}', text, startIndex);
            Assert.That(actual, Is.EqualTo(expected));
        }

        [TestCase("")]
        [TestCase("         ")]
        [TestCase("namespace name{body}",
            "name", "body")]
        [TestCase("   namespace name{body}",
            "name", "body")]
        [TestCase("   namespace name{  body  }",
            "name", "body")]
        [TestCase("   namespace   name  {body}  ",
            "name", "body")]

        [TestCase("namespace name::nestedName{body}",
            "name::nestedName", "body")]
        [TestCase("namespace name{test namespace nestedName {body}}",
            "name", "test",
            "name::nestedName", "body")]

        [TestCase(@"""namespace name{body}""")]
        [TestCase(@""" ""namespace name{body}",
            "name", "body")]
        [TestCase(@"""  \""namespace name{body}""")]
        [TestCase(@"R""test(  ""namespace name{body})test""")]

        [TestCase(@"//namespace name{body}")]
        [TestCase("//\r namespace name{body}",
            "name", "body")]
        [TestCase(@"/*namespace name{body}*/")]
        [TestCase(@"/* */ namespace name{body}",
            "name", "body")]
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
    }
}