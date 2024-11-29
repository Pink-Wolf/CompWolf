#include <private/vulkan_programs/vulkan_gpu_semaphore.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <limits>

namespace compwolf::vulkan
{
	/******************************** constructors ********************************/

	vulkan_gpu_semaphore::vulkan_gpu_semaphore(vulkan_gpu_connection& target_gpu)
	{
		_gpu = &target_gpu;
		auto logicDevice = to_vulkan(gpu().vulkan_device());

		VkSemaphoreCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		};

		VkSemaphore semaphore;
		auto result = vkCreateSemaphore(logicDevice, &createInfo, nullptr, &semaphore);

		switch (result)
		{
		case VK_SUCCESS: break;
		default:
			const char* message;
			GET_VULKAN_ERROR_STRING(result, message,
				"Could not create a gpu semaphore: ")
				throw std::runtime_error(message);
		}

		_vulkan_semaphore = unique_deleter_ptr<vulkan_handle::semaphore_t>(from_vulkan(semaphore),
			[logicDevice](vulkan_handle::semaphore s)
			{
				vkDestroySemaphore(logicDevice, to_vulkan(s), nullptr);
			}
		);
	}
}
