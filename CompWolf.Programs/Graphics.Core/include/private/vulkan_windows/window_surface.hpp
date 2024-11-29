#ifndef COMPWOLF_GRAPHICS_WINDOW_SURFACE
#define COMPWOLF_GRAPHICS_WINDOW_SURFACE

#include <windows>
#include <vulkan_graphics_environments>
#include <unique_deleter_ptr>
#include <memory>
#include <owneds>

namespace compwolf::vulkan
{
	/** The surface of a window, as in the actual area that can display a dynamic image.
	 * This should generally not be accessed directly, and should only be constructed by [[vulkan_window]].
	 * @see vulkan_window
	 */
	class window_surface
	{
	private: // fields
		vulkan_gpu_connection* _gpu;

		unique_deleter_ptr<vulkan_handle::surface_t> _vulkan_surface;
		unique_deleter_ptr<vulkan_handle::surface_format_info_t> _format;
		unique_deleter_ptr<vulkan_handle::render_pass_t> _render_pass;

	public: // accessors
		/** Returns the gpu that the window is on.
		 * @uniqueoverload
		 */
		auto gpu() noexcept -> vulkan_gpu_connection& { return *_gpu; }
		/** Returns the gpu that the window is on. */
		auto gpu() const noexcept -> const vulkan_gpu_connection& { return *_gpu; }

		/** Returns whether this is valid, that is one not constructed by the default constructor. */
		operator bool() const noexcept
		{
			return !!_render_pass;
		}

	public: // vulkan-related
		/** Returns the surface's [[vulkan_handle::surface]], representing a VkSurfaceKHR. */
		auto vulkan_surface() const noexcept -> vulkan_handle::surface { return _vulkan_surface.get(); }
		/** Returns the surface's [[vulkan_handle::surface_format_handle]]-pointer. */
		auto vulkan_format() const noexcept -> const vulkan_handle::surface_format_info { return _format.get(); }
		/** Returns the surface's [[vulkan_handle::render_pass]], representing a VkRenderPass. */
		auto vulkan_render_pass() const noexcept -> vulkan_handle::render_pass { return _render_pass.get(); }

	public: // constructor
		/** Constructs an invalid [[window_surface]].
		 * Using this environment, except [[window_surface::operator bool]], is undefined behaviour.
		 * @overload
		 */
		window_surface() = default;
		window_surface(window_surface&&) = default;
		auto operator=(window_surface&&) -> window_surface& = default;

		/** Should be called by [[vulkan_window]]. */
		window_surface(vulkan_graphics_environment*, vulkan_gpu_connection*,
			window_settings&, vulkan_handle::glfw_window);
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SURFACE
