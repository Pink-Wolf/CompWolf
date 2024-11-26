#include <private/vulkan_sync/gpu_fence.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <limits>

namespace compwolf::vulkan
{
	/******************************** constructors ********************************/

	gpu_fence::gpu_fence(const vulkan_gpu_connection& target_gpu, bool signaled)
	{
		_device = &target_gpu;
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
		default: throw std::runtime_error("Could not create gpu fence.");
		}

		_vulkan_fence = unique_deleter_ptr<vulkan_handle::fence_t>(from_vulkan(fence),
			[logicDevice](vulkan_handle::fence f)
			{
				vkDestroyFence(logicDevice, to_vulkan(f), nullptr);
			}
		);
	}

	auto gpu_fence::signaled() const noexcept -> bool
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());
		auto fence = to_vulkan(vulkan_fence());

		return vkWaitForFences(logicDevice, 1, &fence, VK_TRUE, 0) == VK_SUCCESS;
	}

	/******************************** modifiers ********************************/

	void gpu_fence::wait() const noexcept
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());
		auto fence = to_vulkan(vulkan_fence());

		vkWaitForFences(logicDevice, 1, &fence, VK_TRUE, UINT64_MAX);
	}

	void gpu_fence::reset() noexcept
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());
		auto fence = to_vulkan(vulkan_fence());

		vkResetFences(logicDevice, 1, &fence);
	}
}
