#include "private/vulkan_graphics_environments/environment/glfw_environment.hpp"
#include "compwolf_vulkan.hpp"

#include <stdexcept>
#include <sstream>

namespace compwolf::vulkan
{
	/******************************** constructors ********************************/

	glfw_environment::glfw_environment(const vulkan_graphics_environment_settings&)
	{
		_not_empty = true;

		auto result = glfwInit();

		if (result != GLFW_TRUE)
		{
			const char* message;
			GET_GLFW_ERROR_STRING(glfwGetError(NULL), message, "Could not set up glfw, used by CompWolf: ")
			throw std::runtime_error(message);
		}
	}

	glfw_environment::glfw_environment(glfw_environment&& other) noexcept
	{
		_not_empty = true;
		other._not_empty = false;
	}
	glfw_environment& glfw_environment::operator=(glfw_environment&& other) noexcept
	{
		_not_empty = true;
		other._not_empty = false;
		return *this;
	}

	/******************************** CompWolf::freeable ********************************/

	void glfw_environment::free() noexcept
	{
		if (empty()) return;

		glfwTerminate();
		_not_empty = false;
	}
}
