#ifndef COMPWOLF_VULKAN_HANDLE
#define COMPWOLF_VULKAN_HANDLE

namespace compwolf::vulkan
{
	/** Contains various types of pointers representing handlers from Vulkan.
	 * These can still be used without linking to any Vulkan libraries.
	 */
	struct vulkan_handle
	{
		struct instance_t;
		/** Represents a VkInstance */
		using instance = instance_t*;

		struct physical_device_t;
		/** Represents a VkPhysicalDevice */
		using physical_device = physical_device_t*;

		struct device_t;
		/** Represents a VkDevice */
		using device = device_t*;

		struct queue_t;
		/** Represents a VkQueue */
		using queue = queue_t*;

		struct vulkan_debug_messenger_t;
		/** Represents a VkDebugUtilsMessengerEXT */
		using vulkan_debug_messenger = vulkan_debug_messenger_t*;
	};
}

#endif // ! COMPWOLF_VULKAN_HANDLE