#include "private/vulkan_graphics_environments/vulkan_graphics_environment.hpp"
#include "compwolf_vulkan.hpp"

#include <atomic>
#include <stdexcept>

namespace compwolf::vulkan
{
	vulkan_graphics_environment_settings vulkan_graphics_environment::_settings;
	glfw_environment vulkan_graphics_environment::_glfw_environment;
	vulkan_environment vulkan_graphics_environment::_vulkan_environment;
	vulkan_debug_environment vulkan_graphics_environment::_vulkan_debug_environment;
	std::vector<vulkan_gpu_connection> vulkan_graphics_environment::_gpus{};

	static int _instanceCount{0};

	vulkan_graphics_environment::vulkan_graphics_environment(vulkan_graphics_environment_settings input_settings)
		: graphics_environment(input_settings)
	{
		_settings = input_settings;

		auto newInstanceCount = ++_instanceCount;
		if (newInstanceCount == 1) try
		{
			_glfw_environment = glfw_environment(_settings);
			_vulkan_environment = vulkan_environment(_settings);
			_vulkan_debug_environment = vulkan_debug_environment(_settings, vulkan_instance());

			{
				auto vkInstance = to_vulkan(vulkan_instance());

				auto physicalDevices = get_size_and_vector<uint32_t, VkPhysicalDevice>(
					[vkInstance](uint32_t* size, VkPhysicalDevice* data)
					{
						auto result = vkEnumeratePhysicalDevices(vkInstance, size, data);
						switch (result)
						{
						case VK_SUCCESS:
						case VK_INCOMPLETE:
							break;
						default:
							const char* message;
							GET_VULKAN_ERROR_STRING(result, message,
								"Could not get the machine's graphics card: ")
								throw std::runtime_error(message);
						}
					}
				);

				_gpus.reserve(physicalDevices.size());
				for (auto& physical_device : physicalDevices)
				{
					auto vulkan_physical_device = from_vulkan(physical_device);
					vulkan_gpu_connection new_gpu(*this, vulkan_physical_device);
					_gpus.push_back(std::move(new_gpu));
				}
			}
		}
		catch (...)
		{
			_gpus.clear();
			_vulkan_debug_environment = vulkan_debug_environment();
			_vulkan_environment = vulkan_environment();
			_glfw_environment = glfw_environment();
			throw;
		}
	}
	vulkan_graphics_environment::vulkan_graphics_environment(vulkan_graphics_environment&& other) noexcept
	{
		if (other) ++_instanceCount;
	}
	vulkan_graphics_environment::~vulkan_graphics_environment() noexcept
	{
		auto newInstanceCount = --_instanceCount;
		if (newInstanceCount == 0)
		{
			_gpus.clear();
			_vulkan_debug_environment = vulkan_debug_environment();
			_vulkan_environment = vulkan_environment();
			_glfw_environment = glfw_environment();
		}
	}

	void vulkan_graphics_environment::update()
	{
		if (!is_this_main_thread()) throw std::logic_error("graphics_environment.update() was called on a thread that is not the main graphics thread.");

		glfwPollEvents();
	}
}
