#ifndef COMPWOLF_GRAPHICS_VULKAN_DEBUG_UTILS
#define COMPWOLF_GRAPHICS_VULKAN_DEBUG_UTILS

#include "compwolf_vulkan.hpp"
#include <type_traits>
#include <sstream>
#include <functional>

namespace compwolf
{
	inline VkBool32 vulkan_debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data
	) {
		const char* severity_string;
		switch (message_severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: severity_string = "verbose"; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: severity_string = "info"; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: severity_string = "warning"; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: severity_string = "error"; break;
		default: severity_string = "unknown_severity"; break;
		}

		const char* type_string;
		switch (message_type)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: type_string = "general"; break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: type_string = "validation"; break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: type_string = "performance"; break;
		default: type_string = "unknown_type"; break;
		}

		std::stringstream messageStream("CompWolf debugger: ");
		messageStream << type_string << ' ' << severity_string << ":\n" << callback_data->pMessage << '\n';

		auto& reporter = *static_cast<const std::function<void(std::string_view)>*>(user_data);
		reporter(messageStream.view());

		return VK_FALSE; // Debug callback must return VK_FALSE
	};
	inline constexpr auto vulkan_debug_messenger_create_info(const std::function<void(std::string_view)>* reporter) -> VkDebugUtilsMessengerCreateInfoEXT
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = vulkan_debug_callback,
		.pUserData = const_cast<std::function<void(std::string_view)>*>(reporter),
		};

		return create_info;
	}
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_DEBUG_UTILS
