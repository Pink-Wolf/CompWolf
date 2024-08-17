#include <graphics> // graphics_environment, graphics_environment_settings
#include <windows> // window, window_settings
using namespace CompWolf;

int main()
{
	graphics_environment environment(graphics_environment_settings{
		.program_name = "Test Program",
		});

	window win(environment, window_settings{
		.name = "Test Window",
		});

	while (win.running())
	{
		environment.update();
	}
}