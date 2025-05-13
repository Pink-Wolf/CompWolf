#include <private/vulkan_shaders/vulkan_shader_internal.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace compwolf::vulkan::internal
{
	/******************************** constructors ********************************/

	vulkan_shader_internal::vulkan_shader_internal(vulkan_gpu_connection& gpu
		, const std::vector<uint32_t>& spirvCode
	) {
		auto logicDevice = to_vulkan(gpu.vulkan_device());

		VkShaderModule shaderModule;
		{
			VkShaderModuleCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = spirvCode.size() * sizeof(uint32_t),
				.pCode = spirvCode.data(),
			};

			auto result = vkCreateShaderModule(logicDevice, &createInfo, nullptr, &shaderModule);

			switch (result)
			{
			case VK_SUCCESS: break;
			case VK_ERROR_INVALID_SHADER_NV:
				throw std::runtime_error("Could not compile the given shader code; it is invalid SPIR-V code.");
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not create a buffer on the GPU: ")
					throw std::runtime_error(message);
			}

			vulkan_shader = unique_deleter_ptr<vulkan_handle::shader_t>(from_vulkan(shaderModule),
				[logicDevice](vulkan_handle::shader s)
				{
					vkDestroyShaderModule(logicDevice, to_vulkan(s), nullptr);
				}
			);
		}
	}
}
