#ifndef COMPWOLF_VULKAN_DEBUG_ENVIRONMENT
#define COMPWOLF_VULKAN_DEBUG_ENVIRONMENT

#include "../vulkan_graphics_environment_settings.hpp"
#include "../vulkan_handle.hpp"
#include <unique_deleter_ptr>
#include <memory>
#include <functional>

namespace compwolf::vulkan
{
	/** Should only be used by [[vulkan_graphics_environment]].
	 * Handles internal debugging of Vulkan-part of [[vulkan_graphics_environment]].
	 * @see vulkan_graphics_environment
	 */
	class vulkan_debug_environment
	{
		unique_deleter_ptr<vulkan_handle::vulkan_debug_messenger_t> _vulkan_debugger;

	public: // constructors
		/** Constructs an invalid [[vulkan_debug_environment]].
		 * Using this is undefined behaviour.
		 * @overload
		 */
		vulkan_debug_environment() noexcept = default;
		vulkan_debug_environment(vulkan_debug_environment&&) noexcept = default;
		vulkan_debug_environment& operator=(vulkan_debug_environment&&) noexcept = default;
		/** This should only be called by [[vulkan_graphics_environment]].
		 * If not run on the main graphics thread, then this will have undefined behaviour.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @warning Constructing multiple [[vulkan_debug_environment]]s has undefined behaviour.
		 */
		vulkan_debug_environment(const vulkan_graphics_environment_settings&, vulkan_handle::instance);
	};
}

#endif // ! COMPWOLF_VULKAN_DEBUG_ENVIRONMENT