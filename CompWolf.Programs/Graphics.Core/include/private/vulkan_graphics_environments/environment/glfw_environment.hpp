#ifndef COMPWOLF_GLFW_ENVIRONMENT
#define COMPWOLF_GLFW_ENVIRONMENT

#include "../vulkan_graphics_environment_settings.hpp"

namespace compwolf::vulkan
{
	/** Should only be used by [[vulkan_graphics_environment]].
	 * Handles the GLFW-part of [[vulkan_graphics_environment]].
	 * @see vulkan_graphics_environment
	 */
	class glfw_environment
	{
	private: // fields
		bool _is_valid = false;

	public: // constructors
		/** Constructs an invalid [[glfw_environment]].
		 * Using this is undefined behaviour.
		 * @overload
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

		~glfw_environment() noexcept;
	};
}

#endif // ! COMPWOLF_GLFW_ENVIRONMENT