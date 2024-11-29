#include "private/vulkan_windows/window_surface.hpp"
#include "compwolf_vulkan.hpp"

#include <stdexcept>
#include <optional>
#include <algorithm>

namespace compwolf::vulkan
{
	static std::optional<float> get_surface_format_score(const VkSurfaceFormatKHR& surface_format);
	static std::optional<vulkan_handle::surface_format_info_t> get_present_device_info(
		const vulkan_gpu_connection& gpu, VkSurfaceKHR surface
	);
	static auto find_best_gpu(vulkan_graphics_environment& environment, window_settings& settings,
		vulkan_handle::surface surface, vulkan_handle::surface_format_info out_info) -> vulkan_gpu_connection*;


	window_surface::window_surface(vulkan_graphics_environment* optional_environment,
		vulkan_gpu_connection* optional_gpu,
		window_settings& settings,
		vulkan_handle::glfw_window window)
	{
		auto vulkan_instance = optional_gpu
			? optional_gpu->vulkan_instance()
			: optional_environment->vulkan_instance();
		auto instance = to_vulkan(vulkan_instance);
		auto glfwWindow = to_vulkan(window);

		VkSurfaceKHR surface;
		{
			auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not create a window's surface: ")
					throw std::runtime_error(message);
			}

			_vulkan_surface = unique_deleter_ptr<vulkan_handle::surface_t>(from_vulkan(surface),
				[instance](vulkan_handle::surface s)
				{
					vkDestroySurfaceKHR(instance, to_vulkan(s), nullptr);
				}
			);
		}

		_format = unique_deleter_ptr<vulkan_handle::surface_format_info_t>(new vulkan_handle::surface_format_info_t(),
			[](vulkan_handle::surface_format_info s)
			{
				delete s;
			}
		);
		auto& surface_format = *_format;
		if (optional_gpu)
		{
			_gpu = optional_gpu;
			auto optional_surface = get_present_device_info(*optional_gpu, surface);
			if (!optional_surface.has_value()) throw std::runtime_error("Could not create a window; its specified gpu cannot draw on the window.");
			surface_format = optional_surface.value();
		}
		else
		{
			_gpu = find_best_gpu(*optional_environment, settings, vulkan_surface(), &surface_format);
			if (!_gpu) throw std::runtime_error("Could not create a window; no suitable gpu.");
		}
		auto logicDevice = to_vulkan(gpu().vulkan_device());

		VkRenderPass renderPass, backgroundRenderPass;
		{
			VkAttachmentDescription colorAttachment{
				.format = surface_format.format.format,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			};

			VkAttachmentReference colorAttachmentReference{
				.attachment = 0,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			};
			VkSubpassDescription subpass{
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.colorAttachmentCount = 1,
				.pColorAttachments = &colorAttachmentReference,
			};

			VkRenderPassCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
				.attachmentCount = 1,
				.pAttachments = &colorAttachment,
				.subpassCount = 1,
				.pSubpasses = &subpass,
			};

			auto result = vkCreateRenderPass(logicDevice, &createInfo, nullptr, &renderPass);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not create a window's \"render pass\" for drawing on it: ")
					throw std::runtime_error(message);
			}

			_render_pass = unique_deleter_ptr<vulkan_handle::render_pass_t>(from_vulkan(renderPass),
				[logicDevice](vulkan_handle::render_pass p)
				{
					vkDestroyRenderPass(logicDevice, to_vulkan(p), nullptr);
				}
			);
		}
	}

	static auto find_best_gpu(vulkan_graphics_environment& environment, window_settings& settings,
		vulkan_handle::surface surface, vulkan_handle::surface_format_info out_info) -> vulkan_gpu_connection*
	{
		auto vkSurface = to_vulkan(surface);

		vulkan_gpu_connection* best_gpu = nullptr;
		float best_gpu_score = -1;
		for (auto& gpu : environment.gpus())
		{
			auto info_container = get_present_device_info(gpu, vkSurface);
			if (!info_container.has_value()) continue;
			auto& info = info_container.value();

			float extent_score = static_cast<float>(info.capabilities.maxImageExtent.width) * info.capabilities.maxImageExtent.height;
			extent_score = std::log2(extent_score);
			float extent_score_step; // step is used to put format_score between steps
			{
				float min = 16.f; // 480i has a value of ~18.4
				float max = 48.f; //   4K has a value of ~29.6
				extent_score_step = 1.f / (max - min);

				extent_score -= min;
				extent_score = static_cast<int>(extent_score) * extent_score_step;
				extent_score = std::clamp(extent_score, 0.f, 1.f);
			}

			float present_mode_score = (info.present_mode == VK_PRESENT_MODE_MAILBOX_KHR) ? 1.f : 0.f;

			float score = 0
				+ get_surface_format_score(info.format).value() * extent_score_step
				+ extent_score
				+ present_mode_score * 2;

			if (score > best_gpu_score)
			{
				best_gpu = &gpu;
				best_gpu_score = score;
				*out_info = std::move(info);
			}
		}

		return best_gpu;
	}
	static std::optional<vulkan_handle::surface_format_info_t> get_present_device_info(
		const vulkan_gpu_connection& gpu, VkSurfaceKHR surface)
	{
		auto physicalDevice = to_vulkan(gpu.vulkan_physical_device());

		vulkan_handle::surface_format_info_t return_value{};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &return_value.capabilities);

		auto formats = get_size_and_vector<uint32_t, VkSurfaceFormatKHR>(
			[physicalDevice, surface](uint32_t* size, VkSurfaceFormatKHR* data)
			{
				auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, size, data);
				switch (result)
				{
				case VK_SUCCESS:
				case VK_INCOMPLETE:
					break;
				default:
					const char* message;
					GET_VULKAN_ERROR_STRING(result, message,
						"Could not get ways to present images on a window: ")
						throw std::runtime_error(message);
				}
			}
		);
		if (formats.empty()) return std::nullopt;

		const VkSurfaceFormatKHR* surfaceFormatPointer = nullptr;
		float surface_format_score = std::numeric_limits<float>::lowest();
		{
			for (auto& surface_format : formats)
			{
				auto score_container = get_surface_format_score(surface_format);
				if (!score_container.has_value()) continue;
				auto score = score_container.value();
				if (score > surface_format_score)
				{
					surfaceFormatPointer = &surface_format;
					surface_format_score = score;
				}
			}
			if (surfaceFormatPointer == nullptr) return std::nullopt;
		}
		return_value.format = *surfaceFormatPointer;

		auto presentModes = get_size_and_vector<uint32_t, VkPresentModeKHR>(
			[physicalDevice, surface](uint32_t* size, VkPresentModeKHR* data)
			{
				auto result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, size, data);
				switch (result)
				{
				case VK_SUCCESS:
				case VK_INCOMPLETE:
					break;
				default:
					const char* message;
					GET_VULKAN_ERROR_STRING(result, message,
						"Could not get information about window's surface (present mode): ")
						throw std::runtime_error(message);
				}
			}
		);
		if (presentModes.empty()) return std::nullopt;

		bool has_better_present_mode = std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.end();
		return_value.present_mode = has_better_present_mode
			? VK_PRESENT_MODE_MAILBOX_KHR // Best, allows triple-buffering
			: VK_PRESENT_MODE_FIFO_KHR; // Second best, always available

		return return_value;
	}
	static std::optional<float> get_surface_format_score(const VkSurfaceFormatKHR& surface_format)
	{

		/* Score for formats:
		 * First prefer red, green, blue, and optionally alpha comes in this order.
		 * Then prefer no alpha.
		 * Then prefer more bits representing RGB.
		 *
		 * Keep score within [0;1[.
		 */
		float format_score;
		switch (surface_format.format)
		{
		default: return std::nullopt;
			// RGB
		case VK_FORMAT_R5G6B5_UNORM_PACK16:			format_score = .300f; break;
		case VK_FORMAT_R8G8B8_UNORM:				format_score = .301f; break;
		case VK_FORMAT_R8G8B8_SNORM:				format_score = .301f; break;
		case VK_FORMAT_R8G8B8_USCALED:				format_score = .301f; break;
		case VK_FORMAT_R8G8B8_SSCALED:				format_score = .301f; break;
		case VK_FORMAT_R8G8B8_UINT:					format_score = .301f; break;
		case VK_FORMAT_R8G8B8_SINT:					format_score = .301f; break;
		case VK_FORMAT_R8G8B8_SRGB:					format_score = .301f; break;
		case VK_FORMAT_R16G16B16_UNORM:				format_score = .302f; break;
		case VK_FORMAT_R16G16B16_SNORM:				format_score = .302f; break;
		case VK_FORMAT_R16G16B16_USCALED:			format_score = .302f; break;
		case VK_FORMAT_R16G16B16_SSCALED:			format_score = .302f; break;
		case VK_FORMAT_R16G16B16_UINT:				format_score = .302f; break;
		case VK_FORMAT_R16G16B16_SINT:				format_score = .302f; break;
		case VK_FORMAT_R16G16B16_SFLOAT:			format_score = .302f; break;
		case VK_FORMAT_R32G32B32_UINT:				format_score = .303f; break;
		case VK_FORMAT_R32G32B32_SINT:				format_score = .303f; break;
		case VK_FORMAT_R32G32B32_SFLOAT:			format_score = .303f; break;
		case VK_FORMAT_R64G64B64_UINT:				format_score = .304f; break;
		case VK_FORMAT_R64G64B64_SINT:				format_score = .304f; break;
		case VK_FORMAT_R64G64B64_SFLOAT:			format_score = .304f; break;
			// RGBA
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16:		format_score = .200f; break;
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16:		format_score = .201f; break;
		case VK_FORMAT_R8G8B8A8_UNORM:				format_score = .202f; break;
		case VK_FORMAT_R8G8B8A8_SNORM:				format_score = .202f; break;
		case VK_FORMAT_R8G8B8A8_USCALED:			format_score = .202f; break;
		case VK_FORMAT_R8G8B8A8_SSCALED:			format_score = .202f; break;
		case VK_FORMAT_R8G8B8A8_UINT:				format_score = .202f; break;
		case VK_FORMAT_R8G8B8A8_SINT:				format_score = .202f; break;
		case VK_FORMAT_R8G8B8A8_SRGB:				format_score = .202f; break;
		case VK_FORMAT_R16G16B16A16_UNORM:			format_score = .203f; break;
		case VK_FORMAT_R16G16B16A16_SNORM:			format_score = .203f; break;
		case VK_FORMAT_R16G16B16A16_USCALED:		format_score = .203f; break;
		case VK_FORMAT_R16G16B16A16_SSCALED:		format_score = .203f; break;
		case VK_FORMAT_R16G16B16A16_UINT:			format_score = .203f; break;
		case VK_FORMAT_R16G16B16A16_SINT:			format_score = .203f; break;
		case VK_FORMAT_R16G16B16A16_SFLOAT:			format_score = .203f; break;
		case VK_FORMAT_R32G32B32A32_UINT:			format_score = .204f; break;
		case VK_FORMAT_R32G32B32A32_SINT:			format_score = .204f; break;
		case VK_FORMAT_R32G32B32A32_SFLOAT:			format_score = .204f; break;
		case VK_FORMAT_R64G64B64A64_UINT:			format_score = .205f; break;
		case VK_FORMAT_R64G64B64A64_SINT:			format_score = .205f; break;
		case VK_FORMAT_R64G64B64A64_SFLOAT:			format_score = .205f; break;
			// RGB, but in different order
		case VK_FORMAT_B5G6R5_UNORM_PACK16:			format_score = .100f; break;
		case VK_FORMAT_B8G8R8_UNORM:				format_score = .101f; break;
		case VK_FORMAT_B8G8R8_SNORM:				format_score = .101f; break;
		case VK_FORMAT_B8G8R8_USCALED:				format_score = .101f; break;
		case VK_FORMAT_B8G8R8_SSCALED:				format_score = .101f; break;
		case VK_FORMAT_B8G8R8_UINT:					format_score = .101f; break;
		case VK_FORMAT_B8G8R8_SINT:					format_score = .101f; break;
		case VK_FORMAT_B8G8R8_SRGB:					format_score = .101f; break;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:		format_score = .102f; break;
			// RGBA, but in different order
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16:		format_score = .000f; break;
		case VK_FORMAT_A4R4G4B4_UNORM_PACK16:		format_score = .000f; break;
		case VK_FORMAT_A4B4G4R4_UNORM_PACK16:		format_score = .000f; break;
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16:		format_score = .001f; break;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16:		format_score = .001f; break;
		case VK_FORMAT_B8G8R8A8_UNORM:				format_score = .002f; break;
		case VK_FORMAT_B8G8R8A8_SNORM:				format_score = .002f; break;
		case VK_FORMAT_B8G8R8A8_USCALED:			format_score = .002f; break;
		case VK_FORMAT_B8G8R8A8_SSCALED:			format_score = .002f; break;
		case VK_FORMAT_B8G8R8A8_UINT:				format_score = .002f; break;
		case VK_FORMAT_B8G8R8A8_SINT:				format_score = .002f; break;
		case VK_FORMAT_B8G8R8A8_SRGB:				format_score = .002f; break;
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32:		format_score = .002f; break;
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32:		format_score = .002f; break;
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32:		format_score = .002f; break;
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:		format_score = .002f; break;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32:		format_score = .002f; break;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32:		format_score = .002f; break;
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:		format_score = .002f; break;
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32:	format_score = .003f; break;
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32:	format_score = .003f; break;
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32:	format_score = .003f; break;
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:	format_score = .003f; break;
		case VK_FORMAT_A2R10G10B10_UINT_PACK32:		format_score = .003f; break;
		case VK_FORMAT_A2R10G10B10_SINT_PACK32:		format_score = .003f; break;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:	format_score = .003f; break;
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32:	format_score = .003f; break;
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32:	format_score = .003f; break;
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:	format_score = .003f; break;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32:		format_score = .003f; break;
		case VK_FORMAT_A2B10G10R10_SINT_PACK32:		format_score = .003f; break;
		}

		float color_space_score;
		switch (surface_format.colorSpace)
		{
		default:											color_space_score = 0; break;
			// sRGB
		case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:				color_space_score = 1; break;
		case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:		color_space_score = 1; break;
		}

		bool is_srgb_format = false;
		switch (surface_format.format)
		{
		case VK_FORMAT_R8G8B8_SRGB:
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_B8G8R8_SRGB:
		case VK_FORMAT_B8G8R8A8_SRGB:
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
			is_srgb_format = true;
			break;
		default: break;
		}

		bool is_srgb_space = false;
		switch (surface_format.format)
		{
		case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
		case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
			is_srgb_space = true;
			break;
		default: break;
		}

		return (0
			+ format_score
			+ ((is_srgb_format == is_srgb_space) ? 1 : 0)
			+ 2 * color_space_score
			) / 4.f; // Keep score within [0;1].
	}
}
