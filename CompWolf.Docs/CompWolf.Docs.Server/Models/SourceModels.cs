namespace CompWolf.Docs.Server.Models
{
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
        public string Description { get; set; } = default!;
        public string Declaration { get; set; } = default!;
    }

    public class SourceEntity
    {
        public string Name { get; set; } = default!;
        public EntityTypes Type { get; set; } = default!;
        public string BriefDescription { get; set; } = default!;
        public SourceDeclaration[] Declarations { get; set; } = default!;
        public SourceEntity[] Members { get; set; } = default!;

        public string ReturnDescription { get; set; } = default!;
        public Dictionary<string, string> ParameterDescriptions { get; set; } = default!;
        public string ThrowDescription { get; set; } = default!;

        public string Namespace { get; set; } = default!;
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
