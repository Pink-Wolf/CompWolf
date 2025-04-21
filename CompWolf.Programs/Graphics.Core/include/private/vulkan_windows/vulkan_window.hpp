#ifndef COMPWOLF_GRAPHICS_VULKAN_WINDOW
#define COMPWOLF_GRAPHICS_VULKAN_WINDOW

#include <windows>
#include <vulkan_graphics_environments>
#include <unique_deleter_ptr>
#include "window_surface.hpp"
#include "window_swapchain.hpp"
#include <vulkan_programs>

namespace compwolf::vulkan
{
	/** Vulkan implementation of [[window]].
	 * @see window
	 */
	class vulkan_window : public window<vulkan_graphics_environment>
	{
	private:
		unique_deleter_ptr<vulkan_handle::glfw_window_t> _glfw_window;
		window_surface _surface;
		window_swapchain _swapchain;

		event<const window_draw_parameters&> _drawing;

	public: // vulkan-related
		/** Returns the surface's [[vulkan_handle::glfw_window]], representing a GLFWwindow-pointer. */
		auto glfw_window() const noexcept -> const vulkan_handle::glfw_window { return _glfw_window.get(); }

		/** Returns the surface of the window, as in the actual area that can display a dynamic image. */
		auto surface() noexcept -> window_surface& { return _surface; }
		/** Returns the surface of the window, as in the actual area that can display a dynamic image. */
		auto surface() const noexcept -> const window_surface& { return _surface; }

		/** Returns the swapchain of the window, as in the actual images that are being drawn before being displaying on the window. */
		auto swapchain() noexcept -> window_swapchain& { return _swapchain; }
		/** Returns the swapchain of the window, as in the actual images that are being drawn before being displaying on the window. */
		auto swapchain() const noexcept -> const window_swapchain& { return _swapchain; }

		/** Returns whether this is valid, that is one not constructed by the default constructor. */
		operator bool() const noexcept
		{
			return !!_glfw_window;
		}

		/** Returns an event invoked when the image is being drawn. */
		auto drawing() const noexcept -> const event<const window_draw_parameters&>&
		{ return _drawing; }

	public: // constructors
		/** Constructs an invalid [[vulkan_window]].
		 * Using this environment is undefined behaviour.
		 * @overload
		 */
		vulkan_window() = default;
		vulkan_window(vulkan_window&&) = default;
		auto operator=(vulkan_window&& other) noexcept -> vulkan_window&
		{
			this->~vulkan_window();
			return *new(this)vulkan_window(std::move(other));
		}

		~vulkan_window()
		{
			int test = 3;
		}

	private:
		vulkan_window(vulkan_graphics_environment*, vulkan_gpu_connection*, window_settings settings);
	public:
		/** Constructs a window on the given gpu, with the given settings.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @throws std::invalid_argument if the given settings have invalid settings.
		 * @overload
		 */
		vulkan_window(vulkan_gpu_connection& gpu, window_settings settings)
			: vulkan_window(nullptr, &gpu, settings)
		{}

		/** Constructs a window with the given settings.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @throws std::invalid_argument if the given settings have invalid settings.
		 * @overload
		 */
		vulkan_window(vulkan_graphics_environment& environment, window_settings settings)
			: vulkan_window(&environment, nullptr, settings)
		{}

	public: // compwolf::window
		/** Makes the window update what is shown on it. */
		void update_image() final;

		/** Closes the window. */
		void close() noexcept final
		{
			window::close();
			*this = vulkan_window();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_WINDOW
