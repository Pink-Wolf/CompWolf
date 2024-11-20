#ifndef COMPWOLF_VULKAN_DEBUG_ENVIRONMENT
#define COMPWOLF_VULKAN_DEBUG_ENVIRONMENT

#include "../vulkan_graphics_environment_settings.hpp"
#include "../vulkan_handle.hpp"
#include <freeables>
#include <memory>
#include <functional>

namespace compwolf::vulkan
{
	/** Should only be used by [[vulkan_graphics_environment]].
	 * Handles internal debugging of Vulkan-part of [[vulkan_graphics_environment]].
	 * @see vulkan_graphics_environment
	 */
	class vulkan_debug_environment : public basic_freeable
	{
		struct vulkan_debug_teardown
		{
			using pointer = vulkan_handle::vulkan_debug_messenger;
			std::function<void(pointer)> teardown_function{};
			void operator()(pointer p) const noexcept
			{
				teardown_function(p);
			}
		};
		using debugger_type = std::unique_ptr<vulkan_handle::vulkan_debug_messenger_t, vulkan_debug_teardown>;
		debugger_type _vulkan_debugger = debugger_type(nullptr, vulkan_debug_teardown());

	public: // constructors
		/** @overload Constructs a freed [[vulkan_debug_environment]].
		 * See [[freeable]] for more information.
		 * @see freeable
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

		~vulkan_debug_environment() noexcept { free(); }

	public: // compwolf::freeable
		/** @see freeable */
		auto empty() const noexcept -> bool final
		{
			return !_vulkan_debugger;
		}
		/** @see freeable */
		void free() noexcept final
		{
			_vulkan_debugger.reset();
		}
	};
}

#endif // ! COMPWOLF_VULKAN_DEBUG_ENVIRONMENT