namespace CompWolf.Docs.Server.Models
{
    public class SimpleApiMember
    {
        public string Name { get; set; } = null!;
        public SimpleApiMember[] Members { get; set; } = null!;
    }
    public class SimpleApiEntity
    {
        public string Name { get; set; } = null!;
        public string BriefDescription { get; set; } = null!;
        public SimpleApiMember[] Members { get; set; } = null!;
    }
    public class SimpleApiHeader
    {
        public string Name { get; set; } = null!;
        public string BriefDescription { get; set; } = null!;
        public SimpleApiEntity[] Entities { get; set; } = null!;
    }
    public class SimpleApiProject
    {
        public string Name { get; set; } = null!;
        public string BriefDescription { get; set; } = null!;
        public SimpleApiHeader[] Headers { get; set; } = null!;
    }
    public class ApiCollection
    {
        public SimpleApiProject[] Projects { get; set; } = null!;
    }
}
