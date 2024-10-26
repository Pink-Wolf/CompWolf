using System.Text.Json.Serialization;

namespace CompWolf.Docs.Server.Models
{
    [JsonConverter(typeof(JsonStringEnumConverter))]
    public enum EntityTypes
    {
        Class,
        Concept,
        Function,
        Variable,
        Enum,
        Alias,
        Macro,
    };

    public class SourceDeclaration
    {
        // Default:
        public string Description { get; set; } = default!;
        public string Declaration { get; set; } = default!;
        // Nested:
        public bool Protected { get; set; } = false;
    }

    public class SourceEntity
    {
        // Default:
        public string Name { get; set; } = default!;
        public EntityTypes Type { get; set; } = default!;
        public string BriefDescription { get; set; } = default!;
        public SourceDeclaration[] Declarations { get; set; } = default!;
        public string Namespace { get; set; } = default!;
        // Class:
        public SourceEntity? Constructors { get; set; } = default!;
        public Dictionary<string, SourceEntity[]> Members { get; set; } = default!;
        // Function:
        public string ReturnDescription { get; set; } = default!;
        public Dictionary<string, string> ParameterDescriptions { get; set; } = default!;
        public string ThrowDescription { get; set; } = default!;
    }

    public class SourceHeader
    {
        public string Name { get; set; } = default!;
        public string BriefDescription { get; set; } = default!;
        public SourceEntity[] Entities { get; set; } = default!;
    }
    public class SourceProject
    {
        public string Name { get; set; } = default!;
        public SourceHeader[] Headers { get; set; } = default!;
    }
    public class SourceCollection
    {
        public SourceProject[] Projects { get; set; } = default!;
    }
}
