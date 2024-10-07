using CompWolf.Docs.Server.Models;
using System.Text.Json;

namespace CompWolf.Docs.Server.Data
{
    public class DocumentationDatabase
    {
        private readonly static JsonSerializerOptions serializerOptions = new JsonSerializerOptions
        {
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        };

        public static string DataPath => "Data/";
        public static string ApiPath => $"{DataPath}api/";
        public static string ExamplePath => $"{DataPath}examples/";

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

        public async Task<string?> GetProjectAsync(string project)
        {
            var path = $"{ApiPath}{project}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }
        public async Task<string?> PostProjectAsync(string data, string project)
        {
            var path = $"{ApiPath}{project}";
            await File.WriteAllTextAsync($"{path}.json", data);
            Directory.CreateDirectory(path);
            return data;
        }

        public async Task<string?> GetHeaderAsync(string project, string header)
        {
            var path = $"{ApiPath}{project}/{header}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }
        public async Task<string?> PostHeaderAsync(string data, string project, string header)
        {
            var directoryPath = $"{ApiPath}{project}";
            if (Directory.Exists(directoryPath) is false) return null;
            var path = $"{directoryPath}/{header}";
            await File.WriteAllTextAsync($"{path}.json", data);
            Directory.CreateDirectory(path);
            return data;
        }

        public async Task<string?> GetEntityAsync(string project, string header, string entity)
        {
            var path = $"{ApiPath}{project}/{header}/{entity}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }
        public async Task<string?> PostEntityAsync(string data, string project, string header, string entity)
        {
            var directoryPath = $"{ApiPath}{project}/{header}";
            if (Directory.Exists(directoryPath) is false) return null;
            var path = $"{directoryPath}/{entity}.json";
            await File.WriteAllTextAsync(path, data);
            return data;
        }

        public async Task<Example?> GetExampleAsync(string name)
        {
            var path = $"{ExamplePath}{name}.json";
            if (File.Exists(path) is false) return null;
            using var stream = File.OpenRead(path);
            var example = (await JsonSerializer.DeserializeAsync<Example>(stream, serializerOptions))!;

            example.Code = await File.ReadAllTextAsync($"{ExamplePath}{example.Code}");
            return example;
        }

        public async Task<string?> GetBriefDescriptionAsync(string path)
        {
            if (File.Exists(path) is false) return null;
            using var stream = File.OpenRead(path);
            JsonDocument doc;
            try
            {
                doc = await JsonDocument.ParseAsync(stream);
            }
            catch (Exception)
            {
                Console.WriteLine($"Error while trying to get brief description from \"{path}\"");
                throw;
            }
            return doc.RootElement.GetProperty("briefDescription").GetString();
        }



        
    }
}
