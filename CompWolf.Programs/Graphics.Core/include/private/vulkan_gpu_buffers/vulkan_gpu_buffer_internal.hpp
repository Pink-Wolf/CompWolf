#ifndef COMPWOLF_GRAPHICS_VULKAN_GPU_BUFFER_INTERNAL
#define COMPWOLF_GRAPHICS_VULKAN_GPU_BUFFER_INTERNAL

#include <vulkan_graphics_environments>
#include <gpu_buffers>
#include <unique_deleter_ptr>
#include <utility>

namespace compwolf::vulkan::internal
{
	/** @hidden */
	class vulkan_gpu_buffer_internal
	{
	public:
		unique_deleter_ptr<vulkan_handle::buffer_t> vulkan_buffer{};
		unique_deleter_ptr<vulkan_handle::memory_t> vulkan_memory{};

		/** The size of a single element in the buffer, in bytes. */
		std::size_t stride{};
		/** The amount of elements in the buffer. */
		std::size_t size{};

	public: // accessors
		/** Returns cpu-access to the buffer's data. */
		auto get_data(vulkan_gpu_connection&) -> void*;
		/** Ends cpu-access to the buffer's data. */
		static void free_data(vulkan_gpu_connection&, vulkan_handle::memory) noexcept;

	public: // constructors
		/** Constructs an invalid [[vulkan_gpu_buffer_internal]].
		 * Using this buffer is undefined behaviour.
		 * @overload
		 */
		vulkan_gpu_buffer_internal() = default;
		vulkan_gpu_buffer_internal(vulkan_gpu_buffer_internal&&) = default;
		auto operator=(vulkan_gpu_buffer_internal&&)->vulkan_gpu_buffer_internal & = default;

		/** Creates a buffer on the given gpu. */
		vulkan_gpu_buffer_internal(vulkan_gpu_connection& gpu
			, gpu_buffer_usage usage_type
			, std::size_t stride, std::size_t size
		);
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_GPU_BUFFER_INTERNAL
