#ifndef COMPWOLF_GRAPHICS_COMPWOLF_VULKAN
#define COMPWOLF_GRAPHICS_COMPWOLF_VULKAN

#include <cstdint> // vulkan usually uses uint32_t, which cstdint defines

#include "private/vulkan_graphics_environments/vulkan_handle.hpp"
#include <concepts>
#include <version_numbers>

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#pragma warning(disable)

namespace compwolf::vulkan
{
#define COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(library, library_type, compwolf_type)			\
	inline auto to_##library(compwolf_type a) { return reinterpret_cast<library_type>(a); }			\
	inline auto from_##library(library_type a) { return reinterpret_cast<compwolf_type>(a); }		\

	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkDebugUtilsMessengerEXT, vulkan_handle::vulkan_debug_messenger)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkInstance, vulkan_handle::instance)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkPhysicalDevice, vulkan_handle::physical_device)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkDevice, vulkan_handle::device)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkQueue, vulkan_handle::queue)

	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkFence, vulkan_handle::fence)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkSemaphore, vulkan_handle::semaphore)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkCommandBuffer, vulkan_handle::command)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkCommandPool, vulkan_handle::command_pool)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkShaderModule, vulkan_handle::shader)

	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, GLFWwindow*, vulkan_handle::glfw_window)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkSurfaceKHR, vulkan_handle::surface)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkRenderPass, vulkan_handle::render_pass)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkSwapchainKHR, vulkan_handle::swapchain)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkFramebuffer, vulkan_handle::frame_buffer)

	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkImageView, vulkan_handle::image_view)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkSampler, vulkan_handle::sampler)
		
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkDeviceMemory, vulkan_handle::memory)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkBuffer, vulkan_handle::buffer)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkImage, vulkan_handle::image)
		
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkPipelineLayout, vulkan_handle::pipeline_layout)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkDescriptorSetLayout, vulkan_handle::descriptor_set_layout)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkDescriptorPool, vulkan_handle::descriptor_pool)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkDescriptorSet, vulkan_handle::descriptor_set)
	COMPWOLF_GRAPHICS_DEFINE_VULKAN_CONVERTERS(vulkan, VkPipeline, vulkan_handle::pipeline)

	inline uint32_t to_vulkan(version_number a) { return VK_MAKE_API_VERSION(0, a.major, a.minor, a.patch); }

	struct vulkan_handle::surface_format_info_t
	{
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR format;
		VkPresentModeKHR present_mode;
	};
		
	/** Invokes the given getter to get the size of its data, and then invokes it again to get the data.
	 * The getter must take a [size_pointer], and [data_pointer], where [size_pointer] and [data_pointer] are pointers to where the getter should write the size/data respectively.
	 * The getter should always output the size.
	 * If [data_pointer] is not null, the size must not be greater than the given size, and should output the data.
	 * @return The data received by the getter, in a vector.
	 * @typeparam SizeType [size_pointer] is of the type SizeType*.
	 * @typeparam T The type of data the getter returns an array of. [data_pointer] is of the type T*.
	 * @typeparam GetterType The type of the getter.
	 */
	template <typename SizeType, typename T, typename GetterType>
		requires (std::invocable<GetterType, SizeType*, T*>)
	auto get_size_and_vector(GetterType getter) -> std::vector<T>
	{
		SizeType size;
		getter(&size, nullptr);
		if (size <= 0) return std::vector<T>();

		std::vector<T> data(size);
		getter(&size, data.data());
		data.resize(size);

		return data;
	}

	template <typename FunctionType>
	FunctionType create_vulkan_function(VkInstance instance, const char* function_name)
	{
		auto function = vkGetInstanceProcAddr(instance, function_name);
		return (FunctionType)function;
	}
#define COMPWOLF_GRAPHICS_DECLARE_AND_CREATE_VULKAN_FUNCTION(instance, function_name)\
	auto function_name = ::compwolf::vulkan::create_vulkan_function<PFN_##function_name>(instance, #function_name)

#define GET_GLFW_ERROR_STRING(code, out, prefix) switch (code) {\
	case GLFW_NO_ERROR: out = prefix "GLFW_NO_ERROR"; break;\
	case GLFW_NOT_INITIALIZED: out = prefix "GLFW_NOT_INITIALIZED"; break;\
	case GLFW_NO_CURRENT_CONTEXT: out = prefix "GLFW_NO_CURRENT_CONTEXT"; break;\
	case GLFW_INVALID_ENUM: out = prefix "GLFW_INVALID_ENUM"; break;\
	case GLFW_INVALID_VALUE: out = prefix "GLFW_INVALID_VALUE"; break;\
	case GLFW_OUT_OF_MEMORY: out = prefix "GLFW_OUT_OF_MEMORY"; break;\
	case GLFW_API_UNAVAILABLE: out = prefix "GLFW_API_UNAVAILABLE"; break;\
	case GLFW_VERSION_UNAVAILABLE: out = prefix "GLFW_VERSION_UNAVAILABLE"; break;\
	case GLFW_PLATFORM_ERROR: out = prefix "GLFW_PLATFORM_ERROR"; break;\
	case GLFW_FORMAT_UNAVAILABLE: out = prefix "GLFW_FORMAT_UNAVAILABLE"; break;\
	case GLFW_NO_WINDOW_CONTEXT: out = prefix "GLFW_NO_WINDOW_CONTEXT"; break;\
	default: out = prefix "Unknown error"; break;\
	}

#define GET_VULKAN_ERROR_STRING(code, out, prefix) switch (code) {\
	case VK_SUCCESS: out = prefix "VK_SUCCESS"; break;\
	case VK_NOT_READY: out = prefix "VK_NOT_READY"; break;\
	case VK_TIMEOUT: out = prefix "VK_TIMEOUT"; break;\
	case VK_EVENT_SET: out = prefix "VK_EVENT_SET"; break;\
	case VK_EVENT_RESET: out = prefix "VK_EVENT_RESET"; break;\
	case VK_INCOMPLETE: out = prefix "VK_INCOMPLETE"; break;\
	case VK_ERROR_OUT_OF_HOST_MEMORY: out = prefix "VK_ERROR_OUT_OF_HOST_MEMORY"; break;\
	case VK_ERROR_OUT_OF_DEVICE_MEMORY: out = prefix "VK_ERROR_OUT_OF_DEVICE_MEMORY"; break;\
	case VK_ERROR_INITIALIZATION_FAILED: out = prefix "VK_ERROR_INITIALIZATION_FAILED"; break;\
	case VK_ERROR_DEVICE_LOST: out = prefix "VK_ERROR_DEVICE_LOST"; break;\
	case VK_ERROR_MEMORY_MAP_FAILED: out = prefix "VK_ERROR_MEMORY_MAP_FAILED"; break;\
	case VK_ERROR_LAYER_NOT_PRESENT: out = prefix "VK_ERROR_LAYER_NOT_PRESENT"; break;\
	case VK_ERROR_EXTENSION_NOT_PRESENT: out = prefix "VK_ERROR_EXTENSION_NOT_PRESENT"; break;\
	case VK_ERROR_FEATURE_NOT_PRESENT: out = prefix "VK_ERROR_FEATURE_NOT_PRESENT"; break;\
	case VK_ERROR_INCOMPATIBLE_DRIVER: out = prefix "VK_ERROR_INCOMPATIBLE_DRIVER"; break;\
	case VK_ERROR_TOO_MANY_OBJECTS: out = prefix "VK_ERROR_TOO_MANY_OBJECTS"; break;\
	case VK_ERROR_FORMAT_NOT_SUPPORTED: out = prefix "VK_ERROR_FORMAT_NOT_SUPPORTED"; break;\
	case VK_ERROR_FRAGMENTED_POOL: out = prefix "VK_ERROR_FRAGMENTED_POOL"; break;\
	case VK_ERROR_UNKNOWN: out = prefix "VK_ERROR_UNKNOWN"; break;\
	case VK_ERROR_OUT_OF_POOL_MEMORY: out = prefix "VK_ERROR_OUT_OF_POOL_MEMORY"; break;\
	case VK_ERROR_INVALID_EXTERNAL_HANDLE: out = prefix "VK_ERROR_INVALID_EXTERNAL_HANDLE"; break;\
	case VK_ERROR_FRAGMENTATION: out = prefix "VK_ERROR_FRAGMENTATION"; break;\
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: out = prefix "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"; break;\
	case VK_PIPELINE_COMPILE_REQUIRED: out = prefix "VK_PIPELINE_COMPILE_REQUIRED"; break;\
	default: out = prefix "Unknown error"; break;\
	}
}

#endif // ! COMPWOLF_GRAPHICS_COMPWOLF_VULKAN
