#ifndef COMPWOLF_VULKAN_GPU_CONNECTION
#define COMPWOLF_VULKAN_GPU_CONNECTION

#include <graphics_environments>
#include <unique_deleter_ptr>
#include "vulkan_handle.hpp"
#include "vulkan_gpu_thread_family.hpp"
#include <vector>

namespace compwolf::vulkan
{
	class vulkan_graphics_environment;

	/** A Vulkan-implementation of [[GpuConnection]].
	 * @see GpuConnection
	 * @see vulkan_graphics_environment
	 */
	class vulkan_gpu_connection
	{
		vulkan_graphics_environment* _environment;
		gpu_work_type_set _work_types{};
		std::vector<vulkan_gpu_thread_family> _thread_families{};

		vulkan_handle::physical_device _vulkan_physical_device{};
		unique_deleter_ptr<vulkan_handle::device_t> _vulkan_device{};

	public: // constructors
		/** Constructs an invalid [[vulkan_gpu_environment]].
		 * Using this is undefined behaviour.
		 * @overload
		 */
		vulkan_gpu_connection() noexcept = default;
		vulkan_gpu_connection(vulkan_gpu_connection&&) = default;
		auto operator=(vulkan_gpu_connection&&) -> vulkan_gpu_connection& = default;

		/** Should be called by [[vulkan_graphics_environment]].
		 * Constructs a [[vulkan_gpu_connection]] representing the given [[vulkan_handle::physical_device]].
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @see gpu_connection
		 */
		vulkan_gpu_connection(vulkan_graphics_environment&, vulkan_handle::physical_device);

	public: // accessors
		auto work_types() const noexcept -> gpu_work_type_set
		{
			return _work_types;
		}

		/** Returns an event that is invoked right before the gpu is destructed. */
		auto destructing() const noexcept -> const event<>&;

		/** Returns the [[input_state]] denoting the inputs that the gpu's [[graphic__environment]] is currently receiving.
		 * @see input_state
		 */
		auto inputs() const noexcept -> const input_state&;
		/** Returns the [[input_state]] denoting the inputs that the gpu's [[graphic__environment]] is currently receiving.
		 * @see input_state
		 */
		auto inputs() noexcept -> input_state&;

	public: // vulkan-specific
		/** Returns the GPU's threads. */
		auto thread_families() const noexcept -> const std::vector<vulkan_gpu_thread_family>&
		{
			return _thread_families;
		}

		/** Returns the [[vulkan_handle::instance]] that the GPU is on. */
		auto vulkan_instance() const noexcept -> vulkan_handle::instance;
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
	};
}

#endif // ! COMPWOLF_VULKAN_GPU_CONNECTION