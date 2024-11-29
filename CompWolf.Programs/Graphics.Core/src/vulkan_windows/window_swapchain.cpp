#include "private/vulkan_windows/window_swapchain.hpp"
#include "compwolf_vulkan.hpp"

#include <stdexcept>
#include <algorithm>
#include <vulkan_programs>

namespace compwolf::vulkan
{
	window_swapchain::window_swapchain(window_settings& settings,
		vulkan_handle::glfw_window window, window_surface& surface)
		: _gpu(&surface.gpu())
	{
		auto glfwWindow = to_vulkan(window);

		auto instance = to_vulkan(gpu().vulkan_instance());
		auto logicDevice = to_vulkan(gpu().vulkan_device());

		auto vkSurface = to_vulkan(surface.vulkan_surface());
		auto& surface_format = *surface.vulkan_format();

		uint32_t width, height;
		{
			auto size = settings.pixel_size;
			width = static_cast<uint32_t>(size.x());
			height = static_cast<uint32_t>(size.y());
		}

		// Swapchain
		VkSwapchainKHR swapchain;
		{
			VkExtent2D surfaceExtent;
			{
				int width, height;
				glfwGetFramebufferSize(glfwWindow, &width, &height);
				surfaceExtent = {
					.width = std::clamp<uint32_t>(width, surface_format.capabilities.minImageExtent.width, surface_format.capabilities.maxImageExtent.width),
					.height = static_cast<uint32_t>(height, surface_format.capabilities.minImageExtent.height, surface_format.capabilities.maxImageExtent.height),
				};
			}
			uint32_t swapchain_image_count = surface_format.present_mode == VK_PRESENT_MODE_MAILBOX_KHR ? 3 : 2;
			{
				auto min = surface_format.capabilities.minImageCount;
				auto max = surface_format.capabilities.maxImageCount; if (max == 0) max = swapchain_image_count;
				swapchain_image_count = std::clamp<uint32_t>(swapchain_image_count, min, max);
			}

			VkSwapchainCreateInfoKHR createInfo{
				.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				.surface = vkSurface,
				.minImageCount = swapchain_image_count,
				.imageFormat = surface_format.format.format,
				.imageColorSpace = surface_format.format.colorSpace,
				.imageExtent = surfaceExtent,
				.imageArrayLayers = 1,
				.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.preTransform = surface_format.capabilities.currentTransform,
				.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				.presentMode = surface_format.present_mode,
				.clipped = VK_TRUE,
				.oldSwapchain = VK_NULL_HANDLE,
			};

			auto result = vkCreateSwapchainKHR(logicDevice, &createInfo, nullptr, &swapchain);

			switch (result)
			{
			case VK_SUCCESS: break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: throw std::runtime_error("Could not create the swapchain for a window; something is already displaying images on the window.");
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not create swapchain for a window: ")
					throw std::runtime_error(message);
			}

			_vulkan_swapchain = unique_deleter_ptr<vulkan_handle::swapchain_t>(from_vulkan(swapchain),
				[logicDevice](vulkan_handle::swapchain s)
				{
					vkDeviceWaitIdle(logicDevice);
					vkDestroySwapchainKHR(logicDevice, to_vulkan(s), nullptr);
				}
			);
		}

		// Frames
		{
			auto images = get_size_and_vector<uint32_t, VkImage>([logicDevice, swapchain](uint32_t* size, VkImage* data)
				{
					auto result = vkGetSwapchainImagesKHR(logicDevice, swapchain, size, data);
					switch (result)
					{
					case VK_SUCCESS:
					case VK_INCOMPLETE:
						break;
					default:
						const char* message;
						GET_VULKAN_ERROR_STRING(result, message,
							"Could not get a window's swapchain's images: ")
							throw std::runtime_error(message);
					}
				}
			);

			_frames = std::vector<swapchain_frame>(images.size());
			for (std::size_t i = 0; i < images.size(); ++i)
			{
				auto& image = images;

				VkImageViewCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.image = images[i],
					.viewType = VK_IMAGE_VIEW_TYPE_2D,
					.format = surface_format.format.format,
					.components{
						.r = VK_COMPONENT_SWIZZLE_IDENTITY,
						.g = VK_COMPONENT_SWIZZLE_IDENTITY,
						.b = VK_COMPONENT_SWIZZLE_IDENTITY,
						.a = VK_COMPONENT_SWIZZLE_IDENTITY,
					},
					.subresourceRange{
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.baseMipLevel = 0,
						.levelCount = 1,
						.baseArrayLayer = 0,
						.layerCount = 1,
					},
				};

				VkImageView view;
				auto result = vkCreateImageView(logicDevice, &createInfo, nullptr, &view);

				switch (result)
				{
				case VK_SUCCESS: break;
				default:
					const char* message;
					GET_VULKAN_ERROR_STRING(result, message,
						"Could not get a window's swapchain's images: ")
						throw std::runtime_error(message);
				}

				_frames[i].image_ptr() = unique_deleter_ptr<vulkan_handle::image_view_t>(from_vulkan(view),
					[logicDevice](vulkan_handle::image_view i)
					{
						vkDeviceWaitIdle(logicDevice);
						vkDestroyImageView(logicDevice, to_vulkan(i), nullptr);
					}
				);
			}

			for (auto& frame : _frames)
			{
				auto swapchainImage = to_vulkan(frame.image());

				VkFramebufferCreateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
					.renderPass = to_vulkan(surface.vulkan_render_pass()),
					.attachmentCount = 1,
					.pAttachments = &swapchainImage,
					.width = width,
					.height = height,
					.layers = 1,
				};

				VkFramebuffer framebuffer;
				auto result = vkCreateFramebuffer(logicDevice, &create_info, nullptr, &framebuffer);

				switch (result)
				{
				case VK_SUCCESS: break;
				default:
					const char* message;
					GET_VULKAN_ERROR_STRING(result, message,
						"Could not set up a framebuffer for a window's swapchain: ")
						throw std::runtime_error(message);
				}

				frame.frame_buffer_ptr() = unique_deleter_ptr<vulkan_handle::frame_buffer_t>(from_vulkan(framebuffer),
					[logicDevice](vulkan_handle::frame_buffer f)
					{
						vkDeviceWaitIdle(logicDevice);
						vkDestroyFramebuffer(logicDevice, to_vulkan(f), nullptr);
					}
				);
			}

			for (auto& frame : _frames)
			{
				frame.draw_manager() = vulkan_gpu_program_manager::new_manager_for(gpu()
					, gpu_program_manager_settings
					{
						.type = { gpu_work_type::draw, gpu_work_type::present },
					}
				);
			}
		}

		to_next_frame();
	}

	void window_swapchain::to_next_frame()
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());
		auto swapchain = to_vulkan(vulkan_swapchain());

		uint32_t index;
		vulkan_gpu_fence fence(gpu());
		auto result = vkAcquireNextImageKHR(logicDevice, swapchain, UINT64_MAX, VK_NULL_HANDLE, to_vulkan(fence.vulkan_fence()), &index);

		switch (result)
		{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;
		default:
			const char* message;
			GET_VULKAN_ERROR_STRING(result, message,
				"Could not get the next frame image for a window: ")
				throw std::runtime_error(message);
		}

		_current_frame_index = static_cast<std::size_t>(index);

		fence.wait();
		current_frame().draw_manager().wait();
	}
}
