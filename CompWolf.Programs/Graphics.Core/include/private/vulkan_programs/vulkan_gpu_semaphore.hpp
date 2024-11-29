#ifndef COMPWOLF_GRAPHICS_VULKAN_GPU_SEMAPHORE
#define COMPWOLF_GRAPHICS_VULKAN_GPU_SEMAPHORE

#include <vulkan_graphics_environments>
#include <unique_deleter_ptr>

namespace compwolf::vulkan
{
	/* A "semaphore", which allows synchronization between work on the gpu. */
	class vulkan_gpu_semaphore
	{
	private: // fields
		vulkan_gpu_connection* _gpu;
		/* The vulkan_gpu_semaphore, representing a VkSemaphore. */
		unique_deleter_ptr<vulkan_handle::semaphore_t> _vulkan_semaphore;

		/** Returns whether this is valid, that is one not constructed by the default constructor. */
		operator bool() const noexcept
		{
			return !!_vulkan_semaphore;
		}

	public: // accessors
		/** Returns the gpu that the semaphore is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> vulkan_gpu_connection& { return *_gpu; }
		/** Returns the gpu that the semaphore is on. */
		auto gpu() const noexcept -> const vulkan_gpu_connection& { return *_gpu; }

	public: // vulkan-related
		/* Returns the vulkan_gpu_semaphore, representing a VkSemaphore. */
		auto vulkan_semaphore() const noexcept -> vulkan_handle::semaphore { return _vulkan_semaphore.get(); }

	public: // constructors
		/** Constructs an invalid [[vulkan_gpu_semaphore]].
		 * Using this semaphore, except [[vulkan_gpu_semaphore::operator bool]], is undefined behaviour.
		 * @overload
		 */
		vulkan_gpu_semaphore() = default;
		vulkan_gpu_semaphore(vulkan_gpu_semaphore&&) = default;
		auto operator=(vulkan_gpu_semaphore&&) -> vulkan_gpu_semaphore& = default;

		/* Creates a semaphore for the given gpu.
		 * @throws std::runtime_error if there was an error during creation of the semaphore due to causes outside of the program.
		 */
		vulkan_gpu_semaphore(vulkan_gpu_connection&);
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_GPU_SEMAPHORE
