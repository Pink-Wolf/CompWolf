#ifndef COMPWOLF_VULKAN_GPU_THREAD
#define COMPWOLF_VULKAN_GPU_THREAD

#include "vulkan_handle.hpp"

namespace compwolf::vulkan
{
	/** Aggregate representing a thread on a GPU.
	 * In Vulkan's termonology, specifically represents a queue.
	 * @see vulkan_gpu_thread_family
	 */
	struct vulkan_gpu_thread
	{
		/** The actual [[vulkan_handle::queue]] that the [[vulkan_gpu_thread]] represents.
		 * This should only be referenced when working with a Vulkan library directly.
		 */
		vulkan_handle::queue queue;

		/** The amount of [[vulkan_gpu_program_manager]] currently on the thread. */
		std::size_t program_manager_count;
	};
}

#endif // ! COMPWOLF_VULKAN_GPU_THREAD