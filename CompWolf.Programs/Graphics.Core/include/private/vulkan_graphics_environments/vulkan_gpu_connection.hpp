#ifndef COMPWOLF_VULKAN_GPU_CONNECTION
#define COMPWOLF_VULKAN_GPU_CONNECTION

#include <graphics_environments>
#include <freeables>
#include "vulkan_handle.hpp"
#include "vulkan_gpu_thread_family.hpp"
#include <vector>

namespace compwolf::vulkan
{
	/** A Vulkan-implementation of [[gpu_connection]].
	 * @see gpu_connection
	 * @see vulkan_graphics_environment
	 */
	class vulkan_gpu_connection : public basic_freeable
	{
		gpu_work_type_set _work_types{};
		std::vector<vulkan_gpu_thread_family> _thread_families{};

		vulkan_handle::instance _vulkan_instance{};
		vulkan_handle::physical_device _vulkan_physical_device{};
		vulkan_handle::device _vulkan_device{};

	public: // constructors
		/** @overload Constructs a freed [[graphics_environment]].
		 * See [[freeable]] for more information.
		 * @see freeable
		 */
		constexpr vulkan_gpu_connection() noexcept = default;
		/** Should be called by [[vulkan_graphics_environment]].
		 * Constructs a [[vulkan_gpu_connection]] representing the given [[vulkan_handle::physical_device]].
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @see vulkan_gpu_connection
		 */
		vulkan_gpu_connection(vulkan_handle::instance, vulkan_handle::physical_device);

		~vulkan_gpu_connection() noexcept { free(); }

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
			return _vulkan_device;
		}

	public: // compwolf::freeable
		/** @see freeable */
		auto empty() const noexcept -> bool final
		{
			return !_vulkan_instance;
		}
		/** @see freeable */
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_VULKAN_GPU_CONNECTION