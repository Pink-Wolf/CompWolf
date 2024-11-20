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
}

#endif // ! COMPWOLF_GRAPHICS_COMPWOLF_VULKAN
