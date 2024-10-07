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
        public SourceDatabase Database = new();

        [HttpGet("source")]
        [ProducesResponseType(200)]
        public async Task<ActionResult<SourceCollection>> GetSourceAsync() => await Database.GetSourceCollection();
    }
}
