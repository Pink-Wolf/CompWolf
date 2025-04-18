#ifndef COMPWOLF_VULKAN_HANDLE
#define COMPWOLF_VULKAN_HANDLE

namespace compwolf::vulkan
{
	/** Contains various types of pointers representing handlers from Vulkan.
	 * These can still be used without linking to any Vulkan libraries.
	 */
	struct vulkan_handle
	{
		/** Dereference type of [[vulkan_handle::instance]]
		 * @see vulkan_handle::instance
		 * @hidden
		 */
		struct instance_t;
		/** Represents a VkInstance */
		using instance = instance_t*;

		/** Dereference type of [[vulkan_handle::physical_device]]
		 * @see vulkan_handle::physical_device
		 * @hidden
		 */
		struct physical_device_t;
		/** Represents a VkPhysicalDevice */
		using physical_device = physical_device_t*;

		/** Dereference type of [[vulkan_handle::device]]
		 * @see vulkan_handle::device
		 * @hidden
		 */
		struct device_t;
		/** Represents a VkDevice */
		using device = device_t*;

		/** Dereference type of [[vulkan_handle::queue]]
		 * @see vulkan_handle::queue
		 * @hidden
		 */
		struct queue_t;
		/** Represents a VkQueue */
		using queue = queue_t*;

		/** Dereference type of [[vulkan_handle::vulkan_debug_messenger]]
		 * @see vulkan_handle::vulkan_debug_messenger
		 * @hidden
		 */
		struct vulkan_debug_messenger_t;
		/** Represents a VkDebugUtilsMessengerEXT */
		using vulkan_debug_messenger = vulkan_debug_messenger_t*;


		/** Dereference type of [[vulkan_handle::fence]]
		 * @see vulkan_handle::fence
		 * @hidden
		 */
		struct fence_t;
		/** Represents a VkFence-pointer */
		using fence = fence_t*;

		/** Dereference type of [[vulkan_handle::semaphore]]
		 * @see vulkan_handle::semaphore
		 * @hidden
		 */
		struct semaphore_t;
		/** Represents a VkSemaphore-pointer */
		using semaphore = semaphore_t*;

		/** Dereference type of [[vulkan_handle::command]]
		 * @see vulkan_handle::command
		 * @hidden
		 */
		struct command_t;
		/** Represents a VkCommandBuffer-pointer */
		using command = command_t*;

		/** Dereference type of [[vulkan_handle::command_pool]]
		 * @see vulkan_handle::command_pool
		 * @hidden
		 */
		struct command_pool_t;
		/** Represents a VkCommandPool-pointer */
		using command_pool = command_pool_t*;


		/** Dereference type of [[vulkan_handle::glfw_window]]
		 * @see vulkan_handle::glfw_window
		 * @hidden
		 */
		struct glfw_window_t;
		/** Represents a GLFWwindow-pointer */
		using glfw_window = glfw_window_t*;

		/** Dereference type of [[vulkan_handle::surface]]
		 * @see vulkan_handle::surface
		 * @hidden
		 */
		struct surface_t;
		/** Represents a VkSurfaceKHR-pointer */
		using surface = surface_t*;

		/** Forward declaration of a struct containing information used by [[window_surface]].
		 * @customcode struct surface_format_info_t{};
		 */
		struct surface_format_info_t;
		/** Pointer for [[vulkan_handle::surface_format_info_t]], a struct containing information used by [[window_surface]]. */
		using surface_format_info = surface_format_info_t*;

		/** Dereference type of [[vulkan_handle::render_pass]]
		 * @see vulkan_handle::render_pass
		 * @hidden
		 */
		struct render_pass_t;
		/** Represents a VkRenderPass-pointer */
		using render_pass = render_pass_t*;

		/** Dereference type of [[vulkan_handle::swapchain]]
		 * @see vulkan_handle::swapchain
		 * @hidden
		 */
		struct swapchain_t;
		/** Represents a VkSwapchainKHR-pointer */
		using swapchain = swapchain_t*;

		/** Dereference type of [[vulkan_handle::frame_buffer]]
		 * @see vulkan_handle::frame_buffer
		 * @hidden
		 */
		struct frame_buffer_t;
		/** Represents a VkFramebuffer-pointer */
		using frame_buffer = frame_buffer_t*;


		/** Dereference type of [[vulkan_handle::image_view]]
		 * @see vulkan_handle::image_view
		 * @hidden
		 */
		struct image_view_t;
		/** Represents a VkImageView-pointer */
		using image_view = image_view_t*;

		/** Dereference type of [[vulkan_handle::sampler]]
		 * @see vulkan_handle::sampler
		 * @hidden
		 */
		struct sampler_t;
		/** Represents a VkSampler-pointer */
		using sampler = sampler_t*;
	};
}

#endif // ! COMPWOLF_VULKAN_HANDLE