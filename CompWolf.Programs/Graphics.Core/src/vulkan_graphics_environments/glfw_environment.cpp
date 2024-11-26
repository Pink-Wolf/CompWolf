#include "private/vulkan_graphics_environments/environment/glfw_environment.hpp"
#include "compwolf_vulkan.hpp"

#include <stdexcept>
#include <sstream>

namespace compwolf::vulkan
{
	/******************************** constructors ********************************/

	glfw_environment::glfw_environment(const vulkan_graphics_environment_settings&)
	{
		_is_valid = true;

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
		_is_valid = true;
		other._is_valid = false;
	}
	glfw_environment& glfw_environment::operator=(glfw_environment&& other) noexcept
	{
		_is_valid = true;
		other._is_valid = false;
		return *this;
	}

	/******************************** CompWolf::freeable ********************************/

	glfw_environment::~glfw_environment() noexcept
	{
		if (!_is_valid) return;

		glfwTerminate();
		_is_valid = false;
	}
}
