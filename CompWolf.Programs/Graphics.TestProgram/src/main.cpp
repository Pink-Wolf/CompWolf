#include <iostream>
#include <vulkan_graphics_environments>

int main()
{
	compwolf::vulkan::vulkan_graphics_environment_settings s{
		.internal_debug_callback = [](std::string_view s) {std::cout << s; }
	};
	compwolf::vulkan::vulkan_graphics_environment e(s);
}
