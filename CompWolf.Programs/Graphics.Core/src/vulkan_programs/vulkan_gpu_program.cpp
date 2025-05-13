#include <private/vulkan_programs/vulkan_gpu_program.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace compwolf::vulkan
{
	/******************************** constructors ********************************/

	vulkan_gpu_program::vulkan_gpu_program(vulkan_gpu_program_manager& manager
		, std::function<void(const vulkan_code_parameters&)> code)
		: gpu_specific_program(manager.gpu())
		, _manager(&manager)
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());

		VkCommandBuffer commandBuffer;
		{
			auto vkCommandPool = to_vulkan(manager.vulkan_pool());
			VkCommandBufferAllocateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = vkCommandPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};

			auto result = vkAllocateCommandBuffers(logicDevice, &createInfo, &commandBuffer);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not set up a gpu program's \"command buffer\": ")
					throw std::runtime_error(message);
			}

			_vulkan_command = unique_deleter_ptr<vulkan_handle::command_t>(from_vulkan(commandBuffer),
				[logicDevice, vkCommandPool](vulkan_handle::command c)
				{
					auto vkCommand = to_vulkan(c);
					vkDeviceWaitIdle(logicDevice);
					vkFreeCommandBuffers(logicDevice, vkCommandPool, 1, &vkCommand);
				}
			);
		}

		{
			{
				VkCommandBufferBeginInfo beginInfo{
					.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				};

				auto result = vkBeginCommandBuffer(commandBuffer, &beginInfo);

				switch (result)
				{
				case VK_SUCCESS: break;
				default:
					const char* message;
					GET_VULKAN_ERROR_STRING(result, message,
						"Could not begin recording commands for a gpu program: ")
						throw std::runtime_error(message);
				}
			}

			vulkan_code_parameters compile_parameter{
				.command = _vulkan_command.get()
			};

			code(compile_parameter);

			{
				auto result = vkEndCommandBuffer(commandBuffer);

				switch (result)
				{
				case VK_SUCCESS: break;
				default:
					const char* message;
					GET_VULKAN_ERROR_STRING(result, message,
						"Could not finish recording commands for a gpu program: ")
						throw std::runtime_error(message);
				}
			}
		}

		// This is seemingly needed to get around compiler bug: https://stackoverflow.com/questions/29459040/why-copy-constructor-is-called-instead-of-move-constructor
		{
			_manager_destructing_key.~event_key<>();
			new(&_manager_destructing_key)event_key(manager.destructing().subscribe([this]()
				{
					this->~vulkan_gpu_program();
					new(this)vulkan_gpu_program();
				}
			));
		}
	}

	/******************************** modifiers ********************************/

	auto vulkan_gpu_program::execute() -> const fence_type&
	{
		auto& thread = manager().thread();
		auto queue = to_vulkan(thread.queue);

		auto oldSemaphore = to_vulkan(manager().last_vulkan_semaphore());
		auto& sync = manager().new_synchronization();
		auto fence = to_vulkan(sync.fence.vulkan_fence());
		auto semaphore = to_vulkan(sync.semaphore.vulkan_semaphore());

		auto vulkanCommand = to_vulkan(_vulkan_command.get());

		{
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			VkSubmitInfo submitInfo{
				.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount = (oldSemaphore == nullptr)
					? static_cast<uint32_t>(0)
					: static_cast<uint32_t>(1),
				.pWaitSemaphores = &oldSemaphore,
				.pWaitDstStageMask = waitStages,
				.commandBufferCount = 1,
				.pCommandBuffers = &vulkanCommand,
				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &semaphore,
			};

			auto result = vkQueueSubmit(queue, 1, &submitInfo, fence);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not execute a gpu program; its commands could not be submitted to the gpu: ")
					throw std::runtime_error(message);
			}
		}

		return sync.fence;
	}
}
