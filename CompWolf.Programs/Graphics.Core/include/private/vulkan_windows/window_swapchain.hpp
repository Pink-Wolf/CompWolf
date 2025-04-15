#ifndef COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN
#define COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN

#include <windows>
#include <vulkan_graphics_environments>
#include "swapchain_frame.hpp"
#include "window_surface.hpp"
#include <unique_deleter_ptr>
#include <events>
#include <vector>
#include <cstddef>

namespace compwolf::vulkan
{
	class vulkan_window;
	/** Arguments for vulkan_window::drawing. */
	struct window_draw_parameters
	{
		/** The window. */
		vulkan_window* target_window;
		/** The frame that is being drawn. */
		swapchain_frame* target_frame;
		/** The index of the frame that is being drawn. */
		std::size_t target_frame_index;
	};

	/** The swapchain of a window, as in the actual images that are being drawn and shown on a window.
	 * This should generally not be accessed directly, and should only be constructed by [[vulkan_window]].
	 * @see vulkan_window
	 */
	class window_swapchain
	{
	private: // fields
		vulkan_gpu_connection* _gpu;
		unique_deleter_ptr<vulkan_handle::swapchain_t> _vulkan_swapchain;
		std::vector<swapchain_frame> _frames;
		std::size_t _current_frame_index;

	public: // accessors
		/** Returns the gpu that the window is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> vulkan_gpu_connection& { return *_gpu; }
		/** Returns the gpu that the window is on. */
		auto gpu() const noexcept -> const vulkan_gpu_connection& { return *_gpu; }

		/** Returns information about the individual images of the swapchain. */
		auto frames() noexcept -> std::vector<swapchain_frame>& { return _frames; }
		/** Returns information about the individual images of the swapchain. */
		auto frames() const noexcept -> const std::vector<swapchain_frame>& { return _frames; }

		/** Returns the index in frames() that current_frame() is at. */
		auto current_frame_index() const noexcept -> std::size_t { return _current_frame_index; }

		/** Returns information about the image on the swapchain that is currently being drawn. */
		auto current_frame() noexcept -> swapchain_frame& { return frames()[current_frame_index()]; }
		/** Returns information about the image on the swapchain that is currently being drawn. */
		auto current_frame() const noexcept -> const swapchain_frame& { return frames()[current_frame_index()]; }

		/** Returns whether this is valid, that is one not constructed by the default constructor. */
		operator bool() const noexcept
		{
			return !!_vulkan_swapchain;
		}

	public: // modifiers
		/** Makes the window display the current frame, and makes a new frame the current one. */
		void to_next_frame();

	public: // vulkan-related
		/** Returns the swapchain's vulkan_swapchain, representing a VkSwapchainKHR. */
		auto vulkan_swapchain() const noexcept -> vulkan_handle::swapchain { return _vulkan_swapchain.get(); }

	public: // constructors
		/** Constructs an invalid [[window_swapchain]].
		 * Using this environment is undefined behaviour.
		 * @overload
		 */
		window_swapchain() = default;
		window_swapchain(window_swapchain&&) = default;
		auto operator=(window_swapchain&&) -> window_swapchain& = default;

		/** Should be called by [[vulkan_window]]. */
		window_swapchain(window_settings&,
			vulkan_handle::glfw_window, window_surface&);
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN
