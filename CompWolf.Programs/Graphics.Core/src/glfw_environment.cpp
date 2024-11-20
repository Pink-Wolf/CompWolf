#include "private/vulkan_graphics_environments/environment/glfw_environment.hpp"
#include "compwolf_vulkan.hpp"

#include <stdexcept>

namespace compwolf::vulkan
{
	/******************************** constructors ********************************/

	glfw_environment::glfw_environment(const vulkan_graphics_environment_settings&)
	{
		_not_empty = true;

		auto result = glfwInit();

		if (result != GLFW_TRUE)
		{
			throw std::runtime_error("Could not set up glfw, used by CompWolf");
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
