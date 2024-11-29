#include "private/vulkan_windows/vulkan_window.hpp"
#include "compwolf_vulkan.hpp"

#include <stdexcept>

namespace compwolf::vulkan
{
	vulkan_window::vulkan_window(vulkan_graphics_environment* environment, vulkan_gpu_connection* gpu, window_settings settings)
		: window(*environment, settings)
	{
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			auto glfwWindow = glfwCreateWindow(pixel_size().x(), pixel_size().y()
				, settings.name.data()
				, nullptr, nullptr);
			if (!glfwWindow)
			{
				auto error_code = glfwGetError(NULL);
				switch (error_code)
				{
				case GLFW_API_UNAVAILABLE: throw std::runtime_error("Could not create a window; the machine does not support the right API.");
				case GLFW_VERSION_UNAVAILABLE: throw std::runtime_error("Could not create a window; the machine does not support the right version of OpenGL or openGL ES.");
				case GLFW_FORMAT_UNAVAILABLE: throw std::runtime_error("Could not create a window; the machine does not support the right pixel format.");
				default:
					const char* message;
					GET_GLFW_ERROR_STRING(error_code, message,
						"Could not create a window: ")
						throw std::runtime_error(message);
				}
			}
			_glfw_window = unique_deleter_ptr<vulkan_handle::glfw_window_t>(from_vulkan(glfwWindow),
				[](vulkan_handle::glfw_window w)
				{
					glfwDestroyWindow(to_vulkan(w));
				}
			);

			glfwSetWindowUserPointer(glfwWindow, this);

			glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* glfwWindow)
				{
					window* this_window = static_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
					this_window->close();
				}
			);

			glfwSetInputMode(glfwWindow, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
			glfwSetKeyCallback(glfwWindow, [](GLFWwindow* glfwWindow, int key, int, int action, int mods)
				{
					window* this_window = static_cast<window*>(glfwGetWindowUserPointer(glfwWindow));
					auto& gpu = const_cast<vulkan_gpu_connection&>(this_window->gpu());

					bool pressed_down;
					switch (action)
					{
					case GLFW_PRESS: pressed_down = true; break;
					case GLFW_RELEASE: pressed_down = false; break;
					default: return;
					}

					bool capitalized
						= ((mods & GLFW_MOD_SHIFT) != 0)
						!= ((mods & GLFW_MOD_CAPS_LOCK) != 0);

					char character;
					if (GLFW_KEY_A <= key && key <= GLFW_KEY_Z) character = 'a' + (key - 65);
					else if (key <= 128) character = key;
					else if (key == GLFW_KEY_ENTER) character = '\n';
					else if (key == GLFW_KEY_TAB) character = '	';
					else character = 0;

					if (character != 0)
					{
						gpu.environment().inputs().set_char_state(character, pressed_down, capitalized);
					}
				}
			);
		}

		_surface = window_surface(environment, gpu, settings, glfw_window());
		gpu = &_surface.gpu();
		set_gpu(gpu);

		_swapchain = window_swapchain(settings, glfw_window(), _surface);
	}

	void vulkan_window::update_image()
	{
		window::update_image();

		auto& frame = swapchain().current_frame();
		auto frame_index = swapchain().current_frame_index();
		_drawing.invoke(window_draw_parameters
			{
				.target_window = this,
				.target_frame = &frame,
				.target_frame_index = frame_index,
			}
		);
		{
			auto& draw_manager = frame.draw_manager();
			auto vkSemaphore = to_vulkan(draw_manager.last_vulkan_semaphore());
			auto vkSwapchain = to_vulkan(swapchain().vulkan_swapchain());
			auto frameIndex = static_cast<uint32_t>(frame_index);
			auto& thread = draw_manager.thread();

			vkSemaphore = nullptr; // !!!

			VkPresentInfoKHR presentInfo{
				.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				.waitSemaphoreCount = (vkSemaphore == nullptr)
					? static_cast<uint32_t>(0)
					: static_cast<uint32_t>(1),
				.pWaitSemaphores = &vkSemaphore,
				.swapchainCount = 1,
				.pSwapchains = &vkSwapchain,
				.pImageIndices = &frameIndex,
			};

			vkQueuePresentKHR(to_vulkan(thread.queue), &presentInfo);
		}

		swapchain().to_next_frame();
	}
}
