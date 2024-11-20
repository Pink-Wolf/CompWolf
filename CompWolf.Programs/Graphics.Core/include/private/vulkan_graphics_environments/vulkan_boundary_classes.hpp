#ifndef COMPWOLF_VULKAN_BOUNDARY_CLASSES
#define COMPWOLF_VULKAN_BOUNDARY_CLASSES

#include <graphics_environments>
#include "vulkan_gpu_connection.hpp"

namespace compwolf::vulkan
{
	/** Contains Vulkan-implementations of [[graphics_boundary_classes]], to be used by [[vulkan_graphics_environment]].
	 * @see vulkan_graphics_environment
	 * @see graphics_boundary_classes
	 */
	using vulkan_boundary_classes = graphics_boundary_classes<vulkan_gpu_connection,
		void,
		void,
		void,
		void>;
}

#endif // ! COMPWOLF_VULKAN_BOUNDARY_CLASSES