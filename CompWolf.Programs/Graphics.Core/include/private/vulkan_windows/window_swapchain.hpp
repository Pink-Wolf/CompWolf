#ifndef COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN
#define COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN

#include <windows>
#include <vulkan_graphics_environments>
#include "swapchain_frame.hpp"
#include "window_surface.hpp"
#include <freeables>
#include <unique_deleter_ptr>
#include <owneds>
#include <vector>
#include <cstddef>

namespace compwolf::vulkan
{
	/** The swapchain of a window, as in the actual images that are being drawn and shown on a window.
	 * This should generally not be accessed directly, and should only be constructed by [[vulkan_window]].
	 * @see vulkan_window
	 */
	class window_swapchain : basic_freeable
	{
	private: // fields
		const vulkan_gpu_connection* _gpu;
		unique_deleter_ptr<vulkan_handle::swapchain_t> _vulkan_swapchain;
		std::vector<swapchain_frame> _frames;
		std::size_t _current_frame_index;

	public: // accessors
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

	private: // modifiers
		/** Makes the window display the current frame, and makes a new frame the current one. */
		void to_next_frame();

	public: // vulkan-related
		/** Returns the swapchain's vulkan_swapchain, representing a VkSwapchainKHR. */
		auto vulkan_swapchain() const noexcept -> vulkan_handle::swapchain { return _vulkan_swapchain.get(); }

	public: // constructors
		/** @overload Constructs a freed [[window_swapchain]].
		 * See [[freeable]] for more information.
		 * @see freeable
		 */
		window_swapchain() = default;
		window_swapchain(window_swapchain&&) = default;
		auto operator=(window_swapchain&&) -> window_swapchain& = default;
		
		window_swapchain(window_settings&,
			vulkan_handle::glfw_window, const window_surface&);
		
	public: // compwolf::freeable
		auto empty() const noexcept -> bool final
		{
			return !_vulkan_swapchain;
		}
		void free() noexcept final
		{
			_frames.clear();
			_vulkan_swapchain.reset();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SWAPCHAIN
