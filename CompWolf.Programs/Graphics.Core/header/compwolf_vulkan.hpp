#ifndef COMPWOLF_GRAPHICS_COMPWOLF_VULKAN
#define COMPWOLF_GRAPHICS_COMPWOLF_VULKAN

#include "private/vulkan_graphics_environments/vulkan_handle.hpp"
#include <concepts>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

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

	inline uint32_t to_vulkan(version_number a) { return VK_MAKE_API_VERSION(0, a.major, a.minor, a.patch); }
		
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
		requires std::invocable<GetterType, SizeType*, T*>
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
	case VK_ERROR_SURFACE_LOST_KHR: out = prefix "VK_ERROR_SURFACE_LOST_KHR"; break;\
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: out = prefix "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"; break;\
	case VK_SUBOPTIMAL_KHR: out = prefix "VK_SUBOPTIMAL_KHR"; break;\
	case VK_ERROR_OUT_OF_DATE_KHR: out = prefix "VK_ERROR_OUT_OF_DATE_KHR"; break;\
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: out = prefix "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; break;\
	case VK_ERROR_VALIDATION_FAILED_EXT: out = prefix "VK_ERROR_VALIDATION_FAILED_EXT"; break;\
	case VK_ERROR_INVALID_SHADER_NV: out = prefix "VK_ERROR_INVALID_SHADER_NV"; break;\
	case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: out = prefix "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR"; break;\
	case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: out = prefix "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR"; break;\
	case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: out = prefix "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR"; break;\
	case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: out = prefix "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR"; break;\
	case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: out = prefix "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR"; break;\
	case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: out = prefix "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR"; break;\
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: out = prefix "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"; break;\
	case VK_ERROR_NOT_PERMITTED_KHR: out = prefix "VK_ERROR_NOT_PERMITTED_KHR"; break;\
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: out = prefix "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"; break;\
	case VK_THREAD_IDLE_KHR: out = prefix "VK_THREAD_IDLE_KHR"; break;\
	case VK_THREAD_DONE_KHR: out = prefix "VK_THREAD_DONE_KHR"; break;\
	case VK_OPERATION_DEFERRED_KHR: out = prefix "VK_OPERATION_DEFERRED_KHR"; break;\
	case VK_OPERATION_NOT_DEFERRED_KHR: out = prefix "VK_OPERATION_NOT_DEFERRED_KHR"; break;\
	case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR: out = prefix "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR"; break;\
	case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: out = prefix "VK_ERROR_COMPRESSION_EXHAUSTED_EXT"; break;\
	case VK_INCOMPATIBLE_SHADER_BINARY_EXT: out = prefix "VK_INCOMPATIBLE_SHADER_BINARY_EXT"; break;\
	default: out = prefix "Unknown error"; break;\
	}
}

#endif // ! COMPWOLF_GRAPHICS_COMPWOLF_VULKAN
