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

        [TestCase("")]
        [TestCase("         ")]
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

            CollectionAssert.AreEquivalent(actual, expected);
        }
    }
}