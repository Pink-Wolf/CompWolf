#ifndef COMPWOLF_GLFW_ENVIRONMENT
#define COMPWOLF_GLFW_ENVIRONMENT

#include "../vulkan_graphics_environment_settings.hpp"
#include <freeables>

namespace compwolf::vulkan
{
	/** Should only be used by [[vulkan_graphics_environment]].
	 * Handles the GLFW-part of [[vulkan_graphics_environment]].
	 * @see vulkan_graphics_environment
	 */
	class glfw_environment : public basic_freeable
	{
	private: // fields
		bool _not_empty = false;

	public: // constructors
		/** @overload Constructs a freed [[glfw_handle]].
		 * See [[freeable]] for more information.
		 * @see freeable
		 */
		glfw_environment() = default;
		glfw_environment(glfw_environment&&) noexcept;
		glfw_environment& operator=(glfw_environment&&) noexcept;
		/** This should only be called by [[vulkan_graphics_environment]].
		 * If not run on the main graphics thread, then this will have undefined behaviour.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @warning Constructing multiple [[glfw_handle]]s has undefined behaviour.
		 */
		glfw_environment(const vulkan_graphics_environment_settings&);

		~glfw_environment() noexcept { free(); }

	public: // compwolf::freeable
		/** @see freeable */
		auto empty() const noexcept -> bool final
		{
			return !_not_empty;
		}
		/** @see freeable */
		void free() noexcept final;
	};
}

#endif // ! COMPWOLF_GLFW_ENVIRONMENT