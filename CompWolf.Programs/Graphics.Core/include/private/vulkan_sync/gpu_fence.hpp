#ifndef COMPWOLF_GRAPHICS_GPU_FENCE
#define COMPWOLF_GRAPHICS_GPU_FENCE

#include <vulkan_graphics_environments>
#include <unique_deleter_ptr>

namespace compwolf::vulkan
{
	/** A "fence", which allows the cpu to know when the gpu has finished some work. */
	class gpu_fence
	{
	private:
		const vulkan_gpu_connection* _device;
		unique_deleter_ptr<vulkan_handle::fence_t> _vulkan_fence;

	public: // accessors
		/** Returns the gpu that the fence is on. */
		auto gpu() const noexcept -> const vulkan_gpu_connection& { return *_device; }
		/** Returns true if the work is done, otherwise returns false. */
		auto signaled() const noexcept -> bool;

		/** Returns whether this is valid, that is one not constructed by the default constructor. */
		operator bool() const noexcept
		{
			return !!_vulkan_fence;
		}

	public: // modifiers
		/** Waits until the work is done, and then returns. */
		void wait() const noexcept;
		/** Makes the fence consider its related work not done. */
		void reset() noexcept;

	public: // vulkan-related
		/** Returns the vulkan_fence, representing a VkFence. */
		auto vulkan_fence() const noexcept -> vulkan_handle::fence { return _vulkan_fence.get(); }

	public: // constructors
		/** Constructs an invalid [[gpu_fence]].
		 * Using this fence, except [[gpu_fence::operator bool]], is undefined behaviour.
		 * @overload
		 */
		gpu_fence() = default;
		gpu_fence(gpu_fence&&) = default;
		auto operator=(gpu_fence&&) -> gpu_fence& = default;

		/** Creates a fence for the given gpu.
		 * @param signaled If true, then the fence states that the gpu-work is already finished.
		 * @throws std::runtime_error if there was an error during creation of the fence due to causes outside of the program.
		 */
		gpu_fence(const vulkan_gpu_connection&, bool signaled = false);
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_FENCE
