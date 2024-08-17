using CompWolf.Docs.Server.Data;
using CompWolf.Docs.Server.Models;
using Microsoft.AspNetCore.Mvc;
using System.Text.Json;

namespace CompWolf.Docs.Server.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class ApiController : ControllerBase
    {
        public ApiDatabase Database = new();

        [HttpGet("overview")]
        [ProducesResponseType(200)]
        public async Task<ActionResult<ApiCollection>> GetOverviewAsync() => await Database.GetOverviewAsync();

        [HttpGet("{project}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public async Task<ActionResult<string>> GetProjectAsync([FromRoute] string project)
        {
            var output = await Database.GetProjectAsync(project);
            if (output is null) return NotFound();
            return output;
        }
        [HttpGet("{project}/{header}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public async Task<ActionResult<string>> GetHeaderAsync(
            [FromRoute] string project,
            [FromRoute] string header)
        {
            var output = await Database.GetHeaderAsync(project, header);
            if (output is null) return NotFound();
            return output;
        }
        [HttpGet("{project}/{header}/{entity}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public async Task<ActionResult<string>> GetEntityAsync(
            [FromRoute] string project,
            [FromRoute] string header,
            [FromRoute] string entity)
        {
            var output = await Database.GetEntityAsync(project, header, entity);
            if (output is null) return NotFound();
            return output;
        }

        [HttpPost("{project}")]
        [ProducesResponseType(201)]
        public async Task<ActionResult<string>> PostProjectAsync([FromBody] JsonDocument body,
            [FromRoute] string project)
        {
            var data = body.RootElement.GetRawText();
            var result = await Database.PostProjectAsync(data, project);
            return CreatedAtAction(nameof(GetProjectAsync), new
            {
                project = project!
            }, data);
        }
        [HttpPost("{project}/{header}")]
        [ProducesResponseType(201)]
        [ProducesResponseType(404)]
        public async Task<ActionResult<string>> PostHeaderAsync([FromBody] JsonDocument body,
            [FromRoute] string project,
            [FromRoute] string header)
        {
            var data = body.RootElement.GetRawText();
            var result = await Database.PostHeaderAsync(data, project, header);
            if (result is null) return NotFound();
            return CreatedAtAction(nameof(GetHeaderAsync), new
            {
                project = project!,
                header = header!
            }, data);
        }
        [HttpPost("{project}/{header}/{entity}")]
        [ProducesResponseType(201)]
        [ProducesResponseType(404)]
        public async Task<ActionResult<string>> PostEntityAsync([FromBody] JsonDocument body,
            [FromRoute] string project,
            [FromRoute] string header,
            [FromRoute] string entity)
        {
            var data = body.RootElement.GetRawText();
            var result = await Database.PostEntityAsync(data, project, header, entity);
            if (result is null) return NotFound();
            return CreatedAtAction(nameof(GetEntityAsync), new
            {
                project = project!,
                header = header!,
                entity = entity!
            }, data);
        }
    }
}
