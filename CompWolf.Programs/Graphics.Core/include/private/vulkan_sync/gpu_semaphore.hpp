#ifndef COMPWOLF_GRAPHICS_GPU_SEMAPHORE
#define COMPWOLF_GRAPHICS_GPU_SEMAPHORE

#include <vulkan_graphics_environments>
#include <unique_deleter_ptr>

namespace compwolf::vulkan
{
	/* A "semaphore", which allows synchronization between work on the gpu. */
	class gpu_semaphore
	{
	private: // fields
		/* The gpu that the semaphore is on. */
		const vulkan_gpu_connection* _device;
		/* The vulkan_semaphore, representing a VkSemaphore. */
		unique_deleter_ptr<vulkan_handle::semaphore_t> _vulkan_semaphore;

		/** Returns whether this is valid, that is one not constructed by the default constructor. */
		operator bool() const noexcept
		{
			return !!_vulkan_semaphore;
		}

	public: // accessors
		/* Returns the gpu that the semaphore is on. */
		auto gpu() const noexcept -> const vulkan_gpu_connection& { return *_device; }

	public: // vulkan-related
		/* Returns the vulkan_semaphore, representing a VkSemaphore. */
		auto vulkan_semaphore() const noexcept -> vulkan_handle::semaphore { return _vulkan_semaphore.get(); }

	public: // constructors
		/** Constructs an invalid [[gpu_semaphore]].
		 * Using this semaphore, except [[gpu_semaphore::operator bool]], is undefined behaviour.
		 * @overload
		 */
		gpu_semaphore() = default;
		gpu_semaphore(gpu_semaphore&&) = default;
		auto operator=(gpu_semaphore&&) -> gpu_semaphore& = default;

		/* Creates a semaphore for the given gpu.
		 * @throws std::runtime_error if there was an error during creation of the semaphore due to causes outside of the program.
		 */
		gpu_semaphore(const vulkan_gpu_connection&);
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_SEMAPHORE
