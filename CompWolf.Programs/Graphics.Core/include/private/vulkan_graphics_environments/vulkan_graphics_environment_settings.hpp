#ifndef COMPWOLF_VULKAN_GRAPHICS_ENVIRONMENT_SETTINGS
#define COMPWOLF_VULKAN_GRAPHICS_ENVIRONMENT_SETTINGS

#include <graphics_environments>
#include <string>
#include <version_numbers>
#include <functional>

namespace compwolf::vulkan
{
	/** Aggregate parameter for [[vulkan_graphics_environment]]'s constructor.
	 * @see vulkan_graphics_environment
	 */
	struct vulkan_graphics_environment_settings : public graphics_environment_settings
	{
		/** When not empty, the environment will set up internal debugging, and report debugging messages to this.
		 * This should generally be left empty.
		 */
		std::function<void(std::string_view)> internal_debug_callback;
	};
}

#endif // ! COMPWOLF_VULKAN_GRAPHICS_ENVIRONMENT_SETTINGS