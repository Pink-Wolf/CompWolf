#include <private/vulkan_gpu_buffers/vulkan_gpu_buffer_internal.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace compwolf::vulkan::internal
{
	/******************************** constructors ********************************/

	vulkan_gpu_buffer_internal::vulkan_gpu_buffer_internal(vulkan_gpu_connection& gpu
		, gpu_buffer_usage usage_type
		, std::size_t stride, std::size_t size)
		: stride(stride), size(size)
	{
		auto logicDevice = to_vulkan(gpu.vulkan_device());
		auto physicalDevice = to_vulkan(gpu.vulkan_physical_device());

		VkBuffer vkBuffer;
		{
			VkBufferCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = static_cast<VkDeviceSize>(stride * size),
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			};
			switch (usage_type)
			{
			case gpu_buffer_usage::input_index: createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT; break;
			case gpu_buffer_usage::input: createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; break;
			case gpu_buffer_usage::field: createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; break;
			default: throw std::invalid_argument("Could not create a buffer on the GPU; the given type is unknown.");
			}

			auto result = vkCreateBuffer(logicDevice, &createInfo, nullptr, &vkBuffer);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not create a buffer on the GPU: ")
					throw std::runtime_error(message);
			}

			vulkan_buffer = unique_deleter_ptr<vulkan_handle::buffer_t>(from_vulkan(vkBuffer),
				[logicDevice](vulkan_handle::buffer b)
				{
					vkDestroyBuffer(logicDevice, to_vulkan(b), nullptr);
				}
			);
		}

		VkDeviceMemory vkMemory;
		{
			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(logicDevice, vkBuffer, &memoryRequirements);

			VkPhysicalDeviceMemoryProperties memoryProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

			uint32_t heap_index;
			{
				VkMemoryPropertyFlags requiredProperties
					= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
					;

				for (heap_index = 0; heap_index < memoryProperties.memoryTypeCount; ++heap_index)
				{
					auto& memoryType = memoryProperties.memoryTypes[heap_index];

					if ((memoryRequirements.memoryTypeBits & (1 << heap_index)) == 0) continue;
					if ((memoryType.propertyFlags & requiredProperties) != requiredProperties) continue;
					break;
				}
				if (heap_index == memoryProperties.memoryTypeCount)
					throw std::runtime_error("Could not create a buffer on the GPU: no suitable type of memory on the GPU.");
			}

			VkMemoryAllocateInfo allocateInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = memoryRequirements.size,
				.memoryTypeIndex = heap_index,
			};

			auto result = vkAllocateMemory(logicDevice, &allocateInfo, nullptr, &vkMemory);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not allocate a buffer's memory on the GPU: ")
					throw std::runtime_error(message);
			}

			vulkan_memory = unique_deleter_ptr<vulkan_handle::memory_t>(from_vulkan(vkMemory),
				[logicDevice](vulkan_handle::memory m)
				{
					vkFreeMemory(logicDevice, to_vulkan(m), nullptr);
				}
			);
		}

		{
			auto result = vkBindBufferMemory(logicDevice, vkBuffer, vkMemory, 0);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not bind GPU buffer and its memory together: ")
					throw std::runtime_error(message);
			}
		}
	}

	/******************************** accessors ********************************/

	auto vulkan_gpu_buffer_internal::get_data(vulkan_gpu_connection& gpu) -> void*
	{
		auto logicDevice = to_vulkan(gpu.vulkan_device());
		auto vkMemory = to_vulkan(vulkan_memory.get());

		void* data;
		vkMapMemory(logicDevice, vkMemory, 0, stride * size, 0, &data);
		return data;
	}
	void vulkan_gpu_buffer_internal::free_data(vulkan_gpu_connection& gpu, vulkan_handle::memory memory) noexcept
	{
		auto logicDevice = to_vulkan(gpu.vulkan_device());
		auto vkMemory = to_vulkan(memory);

		vkUnmapMemory(logicDevice, vkMemory);
	}
}
