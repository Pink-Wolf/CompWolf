using CompWolf.Docs.Server.Data;
using CompWolf.Docs.Server.Models;
using Microsoft.AspNetCore.Mvc;

namespace CompWolf.Docs.Server.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class ExamplesController : ControllerBase
    {
        public DocumentationDatabase Database = new();

        [HttpGet("{name}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public async Task<ActionResult<Example>> GetExample([FromRoute] string name)
        {
            var output = await Database.GetExampleAsync(name);
            if (output is null) return NotFound();
            return output;
        }
    }
}
