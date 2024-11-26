#ifndef COMPWOLF_VULKAN_GPU_CONNECTION
#define COMPWOLF_VULKAN_GPU_CONNECTION

#include <graphics_environments>
#include <freeables>
#include <unique_deleter_ptr>
#include "vulkan_handle.hpp"
#include "vulkan_gpu_thread_family.hpp"
#include <vector>

namespace compwolf::vulkan
{
	/** A Vulkan-implementation of [[GpuConnection]].
	 * @see GpuConnection
	 * @see vulkan_graphics_environment
	 */
	class vulkan_gpu_connection : public basic_freeable
	{
		gpu_work_type_set _work_types{};
		std::vector<vulkan_gpu_thread_family> _thread_families{};

		vulkan_handle::instance _vulkan_instance{};
		vulkan_handle::physical_device _vulkan_physical_device{};
		unique_deleter_ptr<vulkan_handle::device_t> _vulkan_device{};

	public: // constructors
		/** Constructs a freed [[vulkan_gpu_connection]].
		 * @see freeable
		 * @overload Constructs a freed [[vulkan_gpu_connection]].
		 */
		vulkan_gpu_connection() noexcept = default;
		vulkan_gpu_connection(vulkan_gpu_connection&&) = default;
		auto operator=(vulkan_gpu_connection&&) -> vulkan_gpu_connection& = default;
		~vulkan_gpu_connection() noexcept { free(); }

		/** Should be called by [[vulkan_graphics_environment]].
		 * Constructs a [[vulkan_gpu_connection]] representing the given [[vulkan_handle::physical_device]].
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @see vulkan_gpu_connection
		 */
		vulkan_gpu_connection(vulkan_handle::instance, vulkan_handle::physical_device);

	public: // accessors
		auto work_types() const noexcept -> gpu_work_type_set
		{
			return _work_types;
		}

	public: // vulkan-specific
		/** Returns the GPU's threads. */
		auto thread_families() const noexcept -> const std::vector<vulkan_gpu_thread_family>&
		{
			return _thread_families;
		}

		/** Returns the [[vulkan_handle::instance]] that the GPU is on. */
		auto vulkan_instance() const noexcept -> vulkan_handle::instance
		{
			return _vulkan_instance;
		}
		/** Returns the [[vulkan_handle::physical_device]] that the [[vulkan_gpu_connection]] represents. */
		auto vulkan_physical_device() const noexcept -> vulkan_handle::physical_device
		{
			return _vulkan_physical_device;
		}
		/** Returns the [[vulkan_handle::device]] that the [[vulkan_gpu_connection]] represents. */
		auto vulkan_device() const noexcept -> vulkan_handle::device
		{
			return _vulkan_device.get();
		}

	public: // compwolf::freeable
		/** @see freeable */
		auto empty() const noexcept -> bool final
		{
			return !_vulkan_device;
		}
		/** @see freeable */
		void free() noexcept final
		{
			_vulkan_device.reset();
		}
	};
}

#endif // ! COMPWOLF_VULKAN_GPU_CONNECTION