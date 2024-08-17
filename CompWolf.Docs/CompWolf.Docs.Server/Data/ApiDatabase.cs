using CompWolf.Docs.Server.Models;
using System.IO;
using System.Reflection.PortableExecutable;
using System.Text.Json;
using System.Threading.Tasks;

namespace CompWolf.Docs.Server.Data
{
    public class ApiDatabase
    {
        private readonly static JsonSerializerOptions serializerOptions = new JsonSerializerOptions
        {
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        };

        public static string DataPath => "Data/";
        public static string ApiPath => $"{DataPath}api/";
        public static string ExamplePath => $"{DataPath}examples/";

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
        public async Task<SimpleApiMember[]> GetMembers(string entityName, JsonElement entity)
        {
            var result = Array.Empty<SimpleApiMember>();

            if (entity.TryGetProperty("constructor", out _))
            {
                result =
                [
                    .. result,
                    new SimpleApiMember()
                    {
                        Name = entityName,
                        Members = [],
                    }
                ];
            }

            if (entity.TryGetProperty("memberGroups", out JsonElement memberGroups))
            {
                int resultI = result.Length;
                {
                    int memberCount = 0;
                    foreach (var memberGroup in memberGroups.EnumerateArray())
                    {
                        memberCount += memberGroup.GetProperty("items").GetArrayLength();
                    }

                    result =
                    [
                        .. result,
                        .. new SimpleApiMember[memberCount]
                    ];
                }

                foreach (var memberGroup in memberGroups.EnumerateArray())
                {
                    foreach (var member in memberGroup.GetProperty("items").EnumerateArray())
                    {
                        var memberName = member.GetProperty("name").GetString()!;
                        result[resultI] = new SimpleApiMember()
                        {
                            Name = memberName,
                            Members = await GetMembers(memberName, member),
                        };
                        ++resultI;
                    }
                }
            }
            return result;
        }
        public async Task<SimpleApiMember[]> GetMembers(string entityName, string filePath)
        {
            using var stream = File.OpenRead(filePath);
            JsonDocument doc;
            try
            {
                doc = await JsonDocument.ParseAsync(stream);
            }
            catch (Exception)
            {
                Console.WriteLine($"Error while trying to get members from \"{filePath}\"");
                throw;
            }

            return await GetMembers(entityName, doc.RootElement);
        }
        public async Task<ApiCollection> GetOverviewAsync()
        {
            ApiCollection collection = new()
            {
                Projects = Directory.GetDirectories(ApiPath).Select(projectDirectory => new SimpleApiProject()
                {
                    Name = Path.GetFileName(projectDirectory)!,
                    Headers = Directory.GetDirectories(projectDirectory).Select(headerDirectory => new SimpleApiHeader()
                    {
                        Name = Path.GetFileName(headerDirectory)!,
                        Entities = Directory.GetFiles(headerDirectory).Select(entityPath => new SimpleApiEntity()
                        {
                            Name = Path.GetFileNameWithoutExtension(entityPath)!,
                            BriefDescription = entityPath,
                        }).ToArray(),
                    }).ToArray(),
                }).ToArray(),
            };

            List<Task<SimpleApiMember[]>> tasks = [];
            foreach (var project in collection.Projects)
            {
                project.BriefDescription = await GetBriefDescriptionAsync($"{ApiPath}{project.Name}.json") ?? "";
                foreach (var header in project.Headers)
                {
                    header.BriefDescription = await GetBriefDescriptionAsync($"{ApiPath}{project.Name}/{header.Name}.json") ?? "";
                    foreach (var entity in header.Entities)
                    {
                        var filePath = entity.BriefDescription;

                        entity.BriefDescription = await GetBriefDescriptionAsync($"{ApiPath}{project.Name}/{header.Name}/{entity.Name}.json") ?? "";

                        tasks.Add(GetMembers(entity.Name, filePath));
                    }
                }
            }

            {
                var taskIterator = tasks.GetEnumerator();
                foreach (var project in collection.Projects)
                {
                    foreach (var header in project.Headers)
                    {
                        foreach (var entity in header.Entities)
                        {
                            taskIterator.MoveNext();
                            entity.Members = await taskIterator.Current;
                        }
                    }
                }
            }

            return collection;
        }
    }
}
