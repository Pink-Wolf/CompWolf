#include "private/vulkan_graphics_environments/environment/vulkan_environment.hpp"
#include "compwolf_vulkan.hpp"
#include "vulkan_debug_utils.hpp"

#include <stdexcept>

namespace compwolf::vulkan
{
	/******************************** constructors ********************************/

	vulkan_environment::vulkan_environment(const vulkan_graphics_environment_settings& settings)
	{
		VkApplicationInfo app_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = settings.program_name.c_str(),
			.applicationVersion = to_vulkan(settings.program_version),
			.pEngineName = "CompWolf",
			.engineVersion = to_vulkan(compwolf_version),
			.apiVersion = VK_API_VERSION_1_3,
		};

		uint32_t glfw_extension_count;
		const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

		std::vector<const char*> validation_layers;
		if (settings.internal_debug_callback)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			validation_layers.push_back("VK_LAYER_KHRONOS_validation");
		}
		auto debugMessengerCreateInfo = vulkan_debug_messenger_create_info(&settings.internal_debug_callback);

		VkInstanceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = settings.internal_debug_callback ? &debugMessengerCreateInfo : nullptr,
			.pApplicationInfo = &app_info,
			.enabledLayerCount = static_cast<uint32_t>(validation_layers.size()),
			.ppEnabledLayerNames = validation_layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		};

		VkInstance instance;
		auto result = vkCreateInstance(&createInfo, nullptr, &instance);

		switch (result)
		{
		case VK_SUCCESS: break;
		case VK_ERROR_LAYER_NOT_PRESENT: throw std::runtime_error("Could not set up vulkan, used by CompWolf; the machine does not have the right layers.");
		case VK_ERROR_EXTENSION_NOT_PRESENT: throw std::runtime_error("Could not set up vulkan, used by CompWolf; the machine does not have the right extensions.");
		case VK_ERROR_INCOMPATIBLE_DRIVER: throw std::runtime_error("Could not set up vulkan, used by CompWolf; the machine has incompatible drivers.");
		default:
			const char* message;
			GET_VULKAN_ERROR_STRING(result, message,
				"Could not set up Vulkan, used by CompWolf.Graphics: ")
				throw std::runtime_error(message);
		}

		_vulkan_instance = unique_deleter_ptr<vulkan_handle::instance_t>(from_vulkan(instance),
			[](vulkan_handle::instance instance)
			{
				return vkDestroyInstance(to_vulkan(instance), nullptr);
			}
		);
	}
}
