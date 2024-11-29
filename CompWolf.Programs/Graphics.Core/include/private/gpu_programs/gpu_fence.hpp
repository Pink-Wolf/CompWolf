#ifndef COMPWOLF_GRAPHICS_GPU_FENCE
#define COMPWOLF_GRAPHICS_GPU_FENCE

#include <vulkan_graphics_environments>

namespace compwolf
{
	/** Boundary class allowing the cpu to wait for the gpu to finish some work.
	 * 
	 * This class is abstract; a derived type should generally implement/override:
	 * * [[window::completed]]
	 * * [[window::wait]]
	 * 
	 * @typeparam GraphicsEnvironmentType The type of [[graphics_environment]] that this window works with.
	 */
	template <typename GraphicsEnvironmentType>
	class gpu_fence
	{
	public:
		/** The type of [[graphics_environment]] that this window works with. */
		using environment_type = GraphicsEnvironmentType;

	private:
		environment_type::gpu_type* _gpu;

	public: // accessors
		/** Returns the gpu that the fence is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> environment_type::gpu_type& { return *_gpu; }
		/** Returns the gpu that the fence is on. */
		auto gpu() const noexcept -> const environment_type::gpu_type& { return *_gpu; }

		/** Returns whether the work is done. */
		virtual auto completed() const noexcept -> bool = 0;

	public: // modifiers
		/** Waits until the work is done, and then returns. */
		virtual void wait() const = 0;

	public: // constructors
		/** Constructs an invalid [[gpu_fence]].
		 * Using this fence is undefined behaviour.
		 * @overload
		 */
		gpu_fence() = default;
		gpu_fence(gpu_fence&&) = default;
		auto operator=(gpu_fence&&) -> gpu_fence& = default;

		/** Creates a fence on the given gpu. */
		gpu_fence(environment_type::gpu_type& gpu) : _gpu(&gpu)
		{}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_FENCE
