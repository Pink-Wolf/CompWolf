#ifndef COMPWOLF_GRAPHICS_SWAPCHAIN_FRAME
#define COMPWOLF_GRAPHICS_SWAPCHAIN_FRAME

#include <vulkan_graphics_environments>
#include <vulkan_programs>

namespace compwolf::vulkan
{
	/* Contains data about an actual image that is being drawn before being displayed on a window. */
	class swapchain_frame
	{
		unique_deleter_ptr<vulkan_handle::image_view_t> _image;
		unique_deleter_ptr<vulkan_handle::frame_buffer_t> _frame_buffer;
		vulkan_gpu_program_manager _draw_manager;

	public: // vulkan-related
		/** Returns the frame's image_view, representing a VkImageView. */
		auto image() const noexcept -> vulkan_handle::image_view { return _image.get(); }
		/** Returns the frame's image_view, representing a VkImageView. */
		auto image_ptr() noexcept -> unique_deleter_ptr<vulkan_handle::image_view_t>& { return _image; }

		/** Returns the frame's frame_buffer, representing a VkFramebuffer. */
		auto frame_buffer() const noexcept -> vulkan_handle::frame_buffer
			{ return _frame_buffer.get(); }
		/** Returns the frame's frame_buffer, representing a VkFramebuffer. */
		auto frame_buffer_ptr() noexcept -> unique_deleter_ptr<vulkan_handle::frame_buffer_t>&
			{ return _frame_buffer; }

		/** Returns the [[vulkan_gpu_program_manager]] for handling drawing on the frame.
		 * @customoverload
		 */
		auto draw_manager() const noexcept -> const vulkan_gpu_program_manager& { return _draw_manager; }
		/** Returns the [[vulkan_gpu_program_manager]] for handling drawing on the frame. */
		auto draw_manager() noexcept -> vulkan_gpu_program_manager& { return _draw_manager; }

	public: // constructors
		/** Constructs an invalid [[swapchain_frame]].
		 * Using this is undefined behaviour.
		 * @overload
		 */
		swapchain_frame() = default;
		swapchain_frame(swapchain_frame&&) = default;
		auto operator=(swapchain_frame&&) -> swapchain_frame& = default;
	};
}

#endif // ! COMPWOLF_GRAPHICS_SWAPCHAIN_FRAME
