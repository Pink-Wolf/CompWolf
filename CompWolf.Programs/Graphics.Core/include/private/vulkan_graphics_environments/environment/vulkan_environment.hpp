#ifndef COMPWOLF_VULKAN_ENVIRONMENT
#define COMPWOLF_VULKAN_ENVIRONMENT

#include "../vulkan_graphics_environment_settings.hpp"
#include "../vulkan_handle.hpp"
#include <freeables>
#include <unique_deleter_ptr>
#include <memory>

namespace compwolf::vulkan
{
	/** Should only be used by [[vulkan_graphics_environment]].
	 * Handles the Vulkan-part of [[vulkan_graphics_environment]].
	 * @see vulkan_graphics_environment
	 */
	class vulkan_environment : public basic_freeable
	{
		unique_deleter_ptr<vulkan_handle::instance_t> _vulkan_instance;

	public: // constructors
		/** @overload Constructs a freed [[vulkan_environment]].
		 * See [[freeable]] for more information.
		 * @see freeable
		 */
		vulkan_environment() = default;
		vulkan_environment(vulkan_environment&&) noexcept = default;
		vulkan_environment& operator=(vulkan_environment&&) noexcept = default;
		/** This should only be called by [[vulkan_graphics_environment]].
		 * If not run on the main graphics thread, then this will have undefined behaviour.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @warning Constructing multiple [[vulkan_environment]]s has undefined behaviour.
		 */
		vulkan_environment(const vulkan_graphics_environment_settings&);

		~vulkan_environment() noexcept { free(); }

	public: // vulkan-specific
		/** Returns the environment's [[vulkan_handle::instance]]. */
		auto vulkan_instance() const noexcept -> vulkan_handle::instance
		{
			return _vulkan_instance.get();
		}

	public: // compwolf::freeable
		/** @see freeable */
		auto empty() const noexcept -> bool final
		{
			return !_vulkan_instance;
		}
		/** @see freeable */
		void free() noexcept final
		{
			_vulkan_instance.reset();
		}
	};
}

#endif // ! COMPWOLF_VULKAN_ENVIRONMENT