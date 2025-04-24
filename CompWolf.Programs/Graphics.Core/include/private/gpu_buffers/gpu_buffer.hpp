#ifndef COMPWOLF_GRAPHICS_GPU_BUFFER
#define COMPWOLF_GRAPHICS_GPU_BUFFER

#include <graphics_environments>
#include <utility>
#include <concepts>

namespace compwolf
{
	/** The different types of buffers, in terms of how they can be used. */
	enum class gpu_buffer_usage
	{
		input,
		input_index,
		field,
	};

	template <typename GraphicsEnvironmentType, gpu_buffer_usage UsageType, typename ValueType>
	class gpu_buffer_data;

	 /** Represents some memory on a gpu.
	  *
		 * This class is abstract; a derived type should generally implement/override:
		 * * [[gpu_buffer::data]]
		 * * [[gpu_buffer::size]]
		 * 
	  * @typeparam GraphicsEnvironmentType The type of [[graphics_environment]] that this buffer works with.
	  * @typeparam UsageType How the data is going to be used.
	  * @typeparam ValueType The type of data in the buffer.
	  */
	template <typename GraphicsEnvironmentType, gpu_buffer_usage UsageType, typename ValueType>
	class gpu_buffer
	{
	public:
		/** The type of [[graphics_environment]] that this buffer works with. */
		using environment_type = GraphicsEnvironmentType;
		/** The type of data in the buffer. */
		using value_type = ValueType;
		/** The type used to have cpu-access to the buffer's data. */
		using access_type = gpu_buffer_data<GraphicsEnvironmentType, UsageType, ValueType>;

	private:
		environment_type::gpu_type* _gpu{};

	public: // accessors
		/** Returns the gpu that the buffer is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> typename environment_type::gpu_type& { return *_gpu; }
		/** Returns the gpu that the buffer is on. */
		auto gpu() const noexcept -> const typename environment_type::gpu_type& { return *_gpu; }

		/** Returns cpu-access to the buffer's data. */
		virtual auto data() -> access_type = 0;
		/** Returns the amount of data in this buffer. */
		virtual auto size() const noexcept -> std::size_t = 0;

		/** Returns how the data can be used. */
		static constexpr auto usage_type() noexcept -> gpu_buffer_usage
		{
			return UsageType;
		}

	public: // constructors
		/** Constructs an invalid [[gpu_buffer]].
		 * Using this buffer is undefined behaviour.
		 * @overload
		 */
		gpu_buffer() = default;
		gpu_buffer(gpu_buffer&&) = default;
		auto operator=(gpu_buffer&&) -> gpu_buffer& = default;

		/** Creates a buffer on the given gpu. */
		gpu_buffer(environment_type::gpu_type& gpu) : _gpu(&gpu)
		{
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_BUFFER
