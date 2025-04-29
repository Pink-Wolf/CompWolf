#include <private/vulkan_drawables/vulkan_brush_internal.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace compwolf::vulkan::internal
{
	/******************************** constructors ********************************/

	vulkan_brush_internal::vulkan_brush_internal(vulkan_gpu_connection& gpu
		, vulkan_brush_info&& info
	) {
		auto instance = to_vulkan(gpu.vulkan_instance());
		auto logicDevice = to_vulkan(gpu.vulkan_device());
		auto physicalDevice = to_vulkan(gpu.vulkan_physical_device());
	}
}
