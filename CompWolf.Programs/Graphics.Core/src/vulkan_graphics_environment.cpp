#include "private/vulkan_graphics_environments/vulkan_graphics_environment.hpp"
#include "compwolf_vulkan.hpp"

#include <atomic>
#include <stdexcept>

namespace compwolf::vulkan
{
	glfw_environment vulkan_graphics_environment::_glfw_environment;
	vulkan_environment vulkan_graphics_environment::_vulkan_environment;
	vulkan_debug_environment vulkan_graphics_environment::_vulkan_debug_environment;
	std::vector<vulkan_gpu_connection> vulkan_graphics_environment::_gpus{};

	static int _instanceCount{0};

	vulkan_graphics_environment::vulkan_graphics_environment(vulkan_graphics_environment_settings settings)
		: graphics_environment(settings)
	{
		auto newInstanceCount = ++_instanceCount;
		if (newInstanceCount == 1) try
		{
			_glfw_environment = glfw_environment(settings);
			_vulkan_environment = vulkan_environment(settings);
			_vulkan_debug_environment = vulkan_debug_environment(settings, vulkan_instance());
		}
		catch (...)
		{
			on_free();
		}
	}

	void vulkan_graphics_environment::update()
	{
		if (!is_this_main_thread()) throw std::logic_error("graphics_environment.update() was called on a thread that is not the main graphics thread.");

		glfwPollEvents();
	}

	void vulkan_graphics_environment::on_free() noexcept
	{
		auto newInstanceCount = --_instanceCount;
		if (newInstanceCount == 0)
		{
			_vulkan_debug_environment.free();
			_vulkan_environment.free();
			_glfw_environment.free();
		}
	}
}
