#ifndef COMPWOLF_GRAPHICS_VULKAN_GPU_BUFFER
#define COMPWOLF_GRAPHICS_VULKAN_GPU_BUFFER

#include <vulkan_graphics_environments>
#include <gpu_buffers>
#include "vulkan_gpu_buffer_internal.hpp"

namespace compwolf::vulkan
{
	/** A Vulkan-implementation of [[gpu_buffer]].
	 * @typeparam UsageType How the data is going to be used.
	 * @typeparam ValueType The type of data in the buffer.
	 * @see gpu_buffer
	 * @see vulkan_graphics_environment
	 */
	template <gpu_buffer_usage UsageType, typename ValueType>
	class vulkan_gpu_buffer
		: public gpu_buffer<vulkan_graphics_environment, UsageType, ValueType>
	{
		using super = gpu_buffer<vulkan_graphics_environment, UsageType, ValueType>;

	private:
		internal::vulkan_gpu_buffer_internal _internal;

	public: // accessors
		/** Returns cpu-access to the buffer's data. */
		auto data() const -> super::access_type final
		{
			return super::access_type(
				this,
				_internal.get_data(super::gpu()), _internal.size,
				[](super::access_type* accessor)
				{
					auto& buffer = *static_cast<vulkan_gpu_buffer*>(accessor.buffer_ptr());
					internal::vulkan_gpu_buffer_internal::free_data(buffer.vulkan_memory());
				}
			);
		}
		/** Returns the amount of data in this buffer. */
		auto size() noexcept -> std::size_t final
		{
			return _internal.size;
		}

	public: // vulkan-specific
		/** Returns the [[vulkan_handle::memory]] that the buffer represents. */
		auto vulkan_memory() const noexcept -> vulkan_handle::memory { return _internal.vulkan_memory.get(); }

		/** Returns the [[vulkan_handle::buffer]] that the buffer represents. */
		auto vulkan_buffer() const noexcept -> vulkan_handle::buffer { return _internal.vulkan_buffer.get(); }

	public: // constructors
		/** Constructs an invalid [[vulkan_gpu_buffer]].
		 * Using this buffer is undefined behaviour.
		 * @overload
		 */
		vulkan_gpu_buffer() = default;
		vulkan_gpu_buffer(vulkan_gpu_buffer&&) = default;
		auto operator=(vulkan_gpu_buffer&&) -> vulkan_gpu_buffer& = default;

		/** Creates a buffer on the given gpu. */
		vulkan_gpu_buffer(vulkan_gpu_connection& gpu, std::size_t size) : super(gpu)
			, _internal(gpu, super::usage_type(), sizeof(super::value_type), size)
		{

		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_GPU_BUFFER
