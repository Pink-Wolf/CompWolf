#include <iostream>
#include <vulkan_graphics_environments>
#include <vulkan_windows>

void debug_callback(std::string_view s)
{
	std::cout << s;
}

int main()
{
	compwolf::vulkan::vulkan_graphics_environment_settings es{
		.internal_debug_callback = &debug_callback
	};
	compwolf::vulkan::vulkan_graphics_environment e(es);

	compwolf::window_settings ws{
		.name = "Window1",
		.pixel_size = {640, 480},
	};
	compwolf::vulkan::vulkan_window w(e, ws);

	e.inputs().char_newly_down().subscribe([](const compwolf::input_key_state& key)
		{
			std::cout << key.character();
		}
	);

	while (w.running())
	{
		e.update();
	}
}
