#include "private/vulkan_graphics_environments/environment/vulkan_debug_environment.hpp"
#include "compwolf_vulkan.hpp"
#include "vulkan_debug_utils.hpp"

#include <stdexcept>

namespace compwolf::vulkan
{
	/******************************** constructors ********************************/

	vulkan_debug_environment::vulkan_debug_environment(const vulkan_graphics_environment_settings& settings, vulkan_handle::instance vulkan_instance)
	{
		if (!settings.internal_debug_callback) return;

		auto instance = to_vulkan(vulkan_instance);

		COMPWOLF_GRAPHICS_DECLARE_AND_CREATE_VULKAN_FUNCTION(instance, vkCreateDebugUtilsMessengerEXT);
		if (vkCreateDebugUtilsMessengerEXT == nullptr)
			throw std::runtime_error("Could not set up CompWolf debugger; could not create constructor for Vulkan debug messenger.");
		COMPWOLF_GRAPHICS_DECLARE_AND_CREATE_VULKAN_FUNCTION(instance, vkDestroyDebugUtilsMessengerEXT);
		if (vkDestroyDebugUtilsMessengerEXT == nullptr)
			throw std::runtime_error("Could not set up CompWolf debugger; could not create destructor for Vulkan debug messenger.");

		auto createInfo = vulkan_debug_messenger_create_info(&settings.internal_debug_callback);

		VkDebugUtilsMessengerEXT debugMessenger;
		auto result = vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);

		switch (result)
		{
		case VK_SUCCESS: break;
		default:
			const char* message;
			GET_VULKAN_ERROR_STRING(result, message,
				"Could not set up CompWolf debugger: could not create Vulkan debug messenger: ")
			throw std::runtime_error(message);
		}


		_vulkan_debugger = unique_deleter_ptr<vulkan_handle::vulkan_debug_messenger_t>(from_vulkan(debugMessenger),
			[vkDestroyDebugUtilsMessengerEXT, instance](vulkan_handle::vulkan_debug_messenger debug_messenger)
			{
				vkDestroyDebugUtilsMessengerEXT(instance, to_vulkan(debug_messenger), nullptr);
			}
		);
	}
}
