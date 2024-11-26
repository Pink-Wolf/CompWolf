#ifndef COMPWOLF_GRAPHICS_GPU_SEMAPHORE
#define COMPWOLF_GRAPHICS_GPU_SEMAPHORE

#include <vulkan_graphics_environments>
#include <freeables>
#include <unique_deleter_ptr>

namespace compwolf::vulkan
{
	/* A "semaphore", which allows synchronization between work on the gpu. */
	class gpu_semaphore : public basic_freeable
	{
	private: // fields
		/* The gpu that the semaphore is on. */
		const vulkan_gpu_connection* _device;
		/* The vulkan_semaphore, representing a VkSemaphore. */
		unique_deleter_ptr<vulkan_handle::semaphore_t> _vulkan_semaphore;

	public: // accessors
		/* Returns the gpu that the semaphore is on. */
		auto gpu() const noexcept -> const vulkan_gpu_connection& { return *_device; }

	public: // vulkan-related
		/* Returns the vulkan_semaphore, representing a VkSemaphore. */
		auto vulkan_semaphore() const noexcept -> vulkan_handle::semaphore { return _vulkan_semaphore.get(); }

	public: // constructors
		/* Constructs a freed gpu_semaphore, as in one that cannot be used. */
		gpu_semaphore() = default;
		gpu_semaphore(gpu_semaphore&&) = default;
		auto operator=(gpu_semaphore&&) -> gpu_semaphore& = default;
		~gpu_semaphore() noexcept { free(); }

		/* Creates a semaphore for the given gpu.
		 * @throws std::runtime_error if there was an error during creation of the semaphore due to causes outside of the program.
		 */
		gpu_semaphore(const vulkan_gpu_connection& target_gpu);

	public: // CompWolf::freeable
		auto empty() const noexcept -> bool final
		{
			return !_vulkan_semaphore;
		}
		void free() noexcept final
		{
			_vulkan_semaphore.reset();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_SEMAPHORE
