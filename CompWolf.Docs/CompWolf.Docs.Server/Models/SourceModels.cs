using System.Text.Json;
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

        public override bool Equals(object? obj)
            => obj is SourceDeclaration other
            && Description == other.Description
            && Declaration == other.Declaration
            && Protected == other.Protected
        ;

        public override int GetHashCode()
            => HashCode.Combine(Description, Declaration, Protected);
    }

    public class SourceEnumMember
    {
        // Default:
        public string Description { get; set; } = default!;
        public string Name { get; set; } = default!;

        public override bool Equals(object? obj)
            => obj is SourceEnumMember other
            && Description == other.Description
            && Name == other.Name
        ;

        public override int GetHashCode()
            => HashCode.Combine(Description, Name);
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
        // Enum:
        public SourceEnumMember[] EnumMembers { get; set; } = default!;

        public override string ToString()
            => JsonSerializer.Serialize(this);

        public override bool Equals(object? obj)
        {
            if (obj is not SourceEntity other) return false;

            var memberCheck = (Members ?? []).Join(other.Members ?? [],
                    x => x.Key, x => x.Key, (lhs, rhs) => lhs.Value.Length == rhs.Value.Length
                    && lhs.Value.Union(rhs.Value).Count() == lhs.Value.Length)
                .ToArray();

            return Name == other.Name
                && Type == other.Type
                && BriefDescription == other.BriefDescription
                && ((Declarations is null) ? other.Declarations is null : other.Declarations is not null
                    && Declarations.SequenceEqual(other.Declarations))
                && Namespace == other.Namespace
                && Constructors == other.Constructors
                && (Members is null ? other.Members is null : other.Members is not null
                    && Members.Count == memberCheck.Length && (memberCheck.Contains(false) is false))
                && ReturnDescription == other.ReturnDescription
                && (ParameterDescriptions is null ? other.ParameterDescriptions is null : other.ParameterDescriptions is not null
                    && ParameterDescriptions.Count == other.ParameterDescriptions.Count
                    && ParameterDescriptions.Union(other.ParameterDescriptions).Count() == ParameterDescriptions.Count)
                && ThrowDescription == other.ThrowDescription
                && ((EnumMembers is null) ? other.EnumMembers is null : other.EnumMembers is not null
                    && EnumMembers.SequenceEqual(other.EnumMembers))
            ;
        }

        public override int GetHashCode()
        {
            HashCode hash = new();
            hash.Add(Name);
            hash.Add(Type);
            hash.Add(BriefDescription);
            hash.Add(Namespace);
            hash.Add(ReturnDescription);
            hash.Add(ThrowDescription);
            hash.Add(Constructors);
            if (Declarations is not null) foreach (var item in Declarations.OrderBy(x => x.Declaration))
                hash.Add(item);
            if (ParameterDescriptions is not null) foreach (var item in ParameterDescriptions.OrderBy(x => x.Key))
            {
                hash.Add(item.Key);
                hash.Add(item.Value);
            }
            if (Members is not null) foreach (var item in Members.OrderBy(x => x.Key))
            {
                hash.Add(item.Key);
                foreach (var nestedItem in item.Value.OrderBy(x => x.Name))
                {
                    hash.Add(nestedItem);
                }
                }
            if (EnumMembers is not null) foreach (var item in EnumMembers)
                    hash.Add(item);
            return hash.ToHashCode();
        }
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
