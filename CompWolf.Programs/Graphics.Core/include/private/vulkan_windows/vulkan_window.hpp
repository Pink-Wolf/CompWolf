#ifndef COMPWOLF_GRAPHICS_VULKAN_WINDOW
#define COMPWOLF_GRAPHICS_VULKAN_WINDOW

#include <windows>
#include <vulkan_graphics_environments>
#include <unique_deleter_ptr>
#include "window_surface.hpp"
#include "window_swapchain.hpp"

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

	public: // constructors
		/** Constructs an invalid [[vulkan_window]].
		 * Using this environment, except [[vulkan_window::operator bool]], is undefined behaviour.
		 * @overload
		 */
		vulkan_window() = default;
		vulkan_window(vulkan_window&&) = default;
		auto operator=(vulkan_window&&) -> vulkan_window& = default;

	private:
		vulkan_window(const vulkan_graphics_environment*, const vulkan_gpu_connection*, window_settings settings);
	public:
		/** Constructs a window on the given gpu, with the given settings.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @throws std::invalid_argument if the given settings have invalid settings.
		 * @overload
		 */
		vulkan_window(const vulkan_gpu_connection& gpu, window_settings settings)
			: vulkan_window(nullptr, &gpu, settings)
		{}

		/** Constructs a window with the given settings.
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @throws std::invalid_argument if the given settings have invalid settings.
		 * @overload
		 */
		vulkan_window(const vulkan_graphics_environment& environment, window_settings settings)
			: vulkan_window(&environment, nullptr, settings)
		{}

	public: // compwolf::window
		/** Makes the window update what is shown on it. */
		void update_image() final;

		/** Closes the window. */
		void close() noexcept final
		{
			window::close();
			_swapchain = window_swapchain();
			_surface = window_surface();
			_glfw_window = nullptr;
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_WINDOW
