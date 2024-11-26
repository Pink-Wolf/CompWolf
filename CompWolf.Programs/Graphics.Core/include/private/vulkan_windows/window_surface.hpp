#ifndef COMPWOLF_GRAPHICS_WINDOW_SURFACE
#define COMPWOLF_GRAPHICS_WINDOW_SURFACE

#include <windows>
#include <vulkan_graphics_environments>
#include <freeables>
#include <unique_deleter_ptr>
#include <memory>
#include <owneds>

namespace compwolf::vulkan
{
	/** The surface of a window, as in the actual area that can display a dynamic image.
	 * This should generally not be accessed directly, and should only be constructed by [[vulkan_window]].
	 * @see vulkan_window
	 */
	class window_surface : public basic_freeable
	{
	private: // fields
		const vulkan_gpu_connection* _gpu;

		unique_deleter_ptr<vulkan_handle::surface_t> _vulkan_surface;
		unique_deleter_ptr<vulkan_handle::surface_format_info_t> _format;
		unique_deleter_ptr<vulkan_handle::render_pass_t> _render_pass;

	public: // accessors
		/** Returns the gpu that the window is on. */
		auto gpu() const noexcept -> const vulkan_gpu_connection& { return *_gpu; }

	public: // vulkan-related
		/** Returns the surface's [[vulkan_handle::surface]], representing a VkSurfaceKHR. */
		auto vulkan_surface() const noexcept -> vulkan_handle::surface { return _vulkan_surface.get(); }
		/** Returns the surface's [[vulkan_handle::surface_format_handle]]-pointer. */
		auto vulkan_format() const noexcept -> const vulkan_handle::surface_format_info { return _format.get(); }
		/** Returns the surface's [[vulkan_handle::render_pass]], representing a VkRenderPass. */
		auto vulkan_render_pass() const noexcept -> vulkan_handle::render_pass { return _render_pass.get(); }

	public: // constructor
		/** Constructs a freed [[window_surface]].
		 * @see freeable
		 * @overload Constructs a freed [[window_surface]].
		 */
		window_surface() = default;
		window_surface(window_surface&&) = default;
		auto operator=(window_surface&&) -> window_surface& = default;
		~window_surface() noexcept { free(); }

	public:
		window_surface(const vulkan_graphics_environment*, const vulkan_gpu_connection*,
			window_settings&, vulkan_handle::glfw_window);

	public: // compwolf::freeable
		auto empty() const noexcept -> bool final
		{
			return !_render_pass;
		}
		void free() noexcept final
		{
			_render_pass.reset();
			_format.reset();
			_vulkan_surface.reset();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SURFACE
