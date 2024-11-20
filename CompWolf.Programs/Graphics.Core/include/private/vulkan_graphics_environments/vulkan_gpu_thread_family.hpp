#ifndef COMPWOLF_VULKAN_GPU_THREAD_FAMILY
#define COMPWOLF_VULKAN_GPU_THREAD_FAMILY

#include "vulkan_gpu_thread.hpp"
#include <graphics_environments>

namespace compwolf::vulkan
{
	/** Aggregate representing a collection of threads on a GPU.
	 * These threads can all perform the same type of work.
	 * In Vulkan's termonology, specifically represents a queue family.
	 * @see vulkan_gpu_thread
	 */
	struct vulkan_gpu_thread_family
	{
		/** The type of work these threads can perform.
		 * @see gpu_work_type_set
		 */
		gpu_work_type_set work_types;
		/** The threads in this family.
		 * @see vulkan_gpu_thread
		 */
		std::vector<vulkan_gpu_thread> threads;
	};
}

#endif // ! COMPWOLF_VULKAN_GPU_THREAD_FAMILY