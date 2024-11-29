#include <private/vulkan_programs/vulkan_gpu_fence.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <limits>

namespace compwolf::vulkan
{
	/******************************** constructors ********************************/

	vulkan_gpu_fence::vulkan_gpu_fence(vulkan_gpu_connection& target_gpu, bool signaled)
		: gpu_fence(target_gpu)
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());

		VkFenceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		};
		if (signaled) createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkFence fence;
		auto result = vkCreateFence(logicDevice, &createInfo, nullptr, &fence);

		switch (result)
		{
		case VK_SUCCESS: break;
		default:
			const char* message;
			GET_VULKAN_ERROR_STRING(result, message,
				"Could not create a gpu fence: ")
				throw std::runtime_error(message);
		}

		_vulkan_fence = unique_deleter_ptr<vulkan_handle::fence_t>(from_vulkan(fence),
			[logicDevice](vulkan_handle::fence f)
			{
				auto fence = to_vulkan(f);
				vkWaitForFences(logicDevice, 1, &fence, VK_TRUE, UINT64_MAX);
				vkDestroyFence(logicDevice, fence, nullptr);
			}
		);
	}

	auto vulkan_gpu_fence::completed() const noexcept -> bool
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());
		auto fence = to_vulkan(vulkan_fence());

		return vkWaitForFences(logicDevice, 1, &fence, VK_TRUE, 0) == VK_SUCCESS;
	}

	/******************************** modifiers ********************************/

	void vulkan_gpu_fence::wait() const noexcept
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());
		auto fence = to_vulkan(vulkan_fence());

		vkWaitForFences(logicDevice, 1, &fence, VK_TRUE, UINT64_MAX);
	}

	void vulkan_gpu_fence::reset() noexcept
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());
		auto fence = to_vulkan(vulkan_fence());

		vkResetFences(logicDevice, 1, &fence);
	}
}
