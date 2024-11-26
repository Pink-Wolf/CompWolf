#ifndef COMPWOLF_GRAPHICS_SWAPCHAIN_FRAME
#define COMPWOLF_GRAPHICS_SWAPCHAIN_FRAME

#include <vulkan_graphics_environments>
#include <freeables>

namespace compwolf::vulkan
{
	/* Contains data about an actual image that is being drawn before being displayed on a window. */
	class swapchain_frame : basic_freeable
	{
		unique_deleter_ptr<vulkan_handle::image_view_t> _image;
		unique_deleter_ptr<vulkan_handle::frame_buffer_t> _frame_buffer;

	public: // vulkan-related
		/** Returns the swapchain's image_view, representing a VkImageView. */
		auto image() const noexcept -> vulkan_handle::image_view { return _image.get(); }
		/** Returns the swapchain's image_view, representing a VkImageView. */
		auto image_ptr() noexcept -> unique_deleter_ptr<vulkan_handle::image_view_t>& { return _image; }

		/** Returns the swapchain's frame_buffer, representing a VkFramebuffer. */
		auto frame_buffer() const noexcept -> vulkan_handle::frame_buffer { return _frame_buffer.get(); }
		/** Returns the swapchain's frame_buffer, representing a VkFramebuffer. */
		auto frame_buffer_ptr() noexcept -> unique_deleter_ptr<vulkan_handle::frame_buffer_t>& { return _frame_buffer; }

	public: // constructors
		/** Constructs a freed [[swapchain_frame]].
		 * @see freeable
		 * @overload Constructs a freed [[swapchain_frame]].
		 */
		swapchain_frame() = default;
		swapchain_frame(swapchain_frame&&) = default;
		auto operator=(swapchain_frame&&) -> swapchain_frame& = default;

	public: // compwolf::freeable
		auto empty() const noexcept -> bool final
		{
			return !(_image || _frame_buffer);
		}
		void free() noexcept final
		{
			_image.reset();
			_frame_buffer.reset();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_SWAPCHAIN_FRAME
