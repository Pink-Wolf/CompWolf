#ifndef COMPWOLF_GRAPHICS_GPU_BUFFER_DATA
#define COMPWOLF_GRAPHICS_GPU_BUFFER_DATA

#include "gpu_buffer.hpp"
#include <span>
#include <owneds>

namespace compwolf
{
	/** Boundary class representing cpu-access to a [[gpu_buffer]]'s data.
	 * This inherits from [[std::span]].
	 *
	 * @typeparam GraphicsEnvironmentType The type of [[graphics_environment]] that this buffer works with.
	 * @typeparam UsageType How the data can be used.
	 * @typeparam ValueType The type of data in the buffer.
	 */
	template <typename GraphicsEnvironmentType, gpu_buffer_usage UsageType, typename ValueType>
	class gpu_buffer_data : public std::span<ValueType, std::dynamic_extent>
	{
	private:
		using super = std::span<ValueType, std::dynamic_extent>;
	public:
		/** The type of [[graphics_environment]] that this buffer works with. */
		using environment_type = GraphicsEnvironmentType;
		/** The type of data in the buffer. */
		using value_type = ValueType;
		/** The type of buffer this provides access to. */
		using buffer_type = gpu_buffer<GraphicsEnvironmentType, UsageType, ValueType>;

	private:
		buffer_type* _buffer{};
		owned<std::function<void(gpu_buffer_data*)>> _on_access_freed{};

	public: // accessors
		/** Returns the buffer whose data this accesses.
		 * @customoverload
		 */
		auto buffer_ptr() noexcept -> buffer_type* { return _buffer; }
		/** Returns the buffer whose data this accesses. */
		auto buffer_ptr() const noexcept -> const buffer_type* { return _buffer; }

		/** Returns the gpu that the buffer is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> typename environment_type::gpu_type& { return buffer_ptr()->gpu(); }
		/** Returns the gpu that the buffer is on. */
		auto gpu() const noexcept -> const typename environment_type::gpu_type& { return buffer_ptr()->gpu(); }

	public: // constructors
		/** Constructs an invalid [[gpu_buffer_data]].
		 * Using this buffer access is undefined behaviour.
		 * @overload
		 */
		gpu_buffer_data() = default;
		gpu_buffer_data(gpu_buffer_data&&) = default;
		auto operator=(gpu_buffer_data&& other) noexcept -> gpu_buffer_data&
		{
			this->~gpu_buffer_data();
			return *new(this)gpu_buffer_data(std::move(other));
		}

		~gpu_buffer_data()
		{
			if (_on_access_freed)
				_on_access_freed(this);
		}

		/** Creates access to the given buffer.
		 * This should only be directly called by an implementation of [[gpu_buffer::data]].
		 * @param data Where the data can actually be accessed, as a span of memory.
		 * @param size The amount of data.
		 * @param on_access_freed Function invoked when the access is ended.
		 */
		gpu_buffer_data(buffer_type* buffer
			, ValueType* data, std::size_t size
			, std::function<void(gpu_buffer_data*)> on_access_freed)
			: super(data, size)
			, _on_access_freed(on_access_freed)
		{
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_BUFFER_DATA
