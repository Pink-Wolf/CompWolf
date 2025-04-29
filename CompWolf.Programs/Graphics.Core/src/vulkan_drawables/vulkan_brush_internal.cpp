#include <private/vulkan_drawables/vulkan_brush_internal.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>
#include <vector>
#include <array>

namespace compwolf::vulkan::internal
{
	/******************************** constructors ********************************/

	vulkan_brush_internal::vulkan_brush_internal(vulkan_gpu_connection& gpu
		, vulkan_brush_info& info
	) {
		auto instance = to_vulkan(gpu.vulkan_instance());
		auto logicDevice = to_vulkan(gpu.vulkan_device());
		auto physicalDevice = to_vulkan(gpu.vulkan_physical_device());

		VkDescriptorSetLayout descriptorSetLayout;
		{
			std::vector<VkDescriptorSetLayoutBinding> uniformBindings;
			uniformBindings.reserve(info.field_primitive_indices.size() * 2);
			for (std::size_t i = 0; i < info.field_primitive_indices.size(); ++i)
			{
				VkDescriptorSetLayoutBinding layoutBinding{
					.binding = static_cast<uint32_t>(info.field_primitive_indices[i]),
					.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.descriptorCount = 1,
				};

				if (info.field_is_input_field[i]) layoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
				if (info.field_is_pixel_field[i]) layoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

				uniformBindings.emplace_back(std::move(layoutBinding));
			}

			VkDescriptorSetLayoutCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.bindingCount = static_cast<uint32_t>(uniformBindings.size()),
				.pBindings = uniformBindings.data(),
			};

			auto result = vkCreateDescriptorSetLayout(logicDevice, &createInfo, nullptr, &descriptorSetLayout);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not create a brush' descriptor set layout on the GPU: ")
					throw std::runtime_error(message);
			}

			vulkan_descriptor_set_layout = unique_deleter_ptr<vulkan_handle::descriptor_set_layout_t>(from_vulkan(descriptorSetLayout),
				[logicDevice](vulkan_handle::descriptor_set_layout l)
				{
					vkDestroyDescriptorSetLayout(logicDevice, to_vulkan(l), nullptr);
				}
			);
		}

		VkPipelineLayout pipelineLayout;
		{
			VkPipelineLayoutCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				.setLayoutCount = 1,
				.pSetLayouts = &descriptorSetLayout,
				.pushConstantRangeCount = 0,
			};

			auto result = vkCreatePipelineLayout(logicDevice, &createInfo, nullptr, &pipelineLayout);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not create a brush' pipeline layout on the GPU: ")
					throw std::runtime_error(message);
			}

			vulkan_pipeline_layout = unique_deleter_ptr<vulkan_handle::pipeline_layout_t>(from_vulkan(pipelineLayout),
				[logicDevice](vulkan_handle::pipeline_layout l)
				{
					vkDestroyPipelineLayout(logicDevice, to_vulkan(l), nullptr);
				}
			);
		}
	}
	
	vulkan_window_brush::vulkan_window_brush(vulkan_window& window
		, vulkan_brush_info& info, vulkan_handle::shader input_shader, vulkan_handle::shader pixel_shader
		, vulkan_handle::descriptor_set_layout descriptor_set_layout, vulkan_handle::pipeline_layout pipeline_layout
	)
	{
		auto& frames = window.swapchain().frames();
		auto renderpass = to_vulkan(window.surface().vulkan_render_pass());

		auto& gpu = window.gpu();
		auto instance = to_vulkan(gpu.vulkan_instance());
		auto logicDevice = to_vulkan(gpu.vulkan_device());
		auto physicalDevice = to_vulkan(gpu.vulkan_physical_device());
		auto descriptorSetLayout = to_vulkan(descriptor_set_layout);
		auto pipelineLayout = to_vulkan(pipeline_layout);

		{
			VkPipelineShaderStageCreateInfo vertexCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = to_vulkan(input_shader),
				.pName = "main",
			};
			VkPipelineShaderStageCreateInfo fragCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = to_vulkan(pixel_shader),
				.pName = "main",
			};
			std::array<VkPipelineShaderStageCreateInfo, 2> stageCreateInfo { std::move(vertexCreateInfo), std::move(fragCreateInfo) };

			VkVertexInputBindingDescription bindingDescriptions{
				.binding = static_cast<uint32_t>(0),
				.stride = static_cast<uint32_t>(info.input_stride),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
			};
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
			{
				attributeDescriptions.reserve(info.input_primitive_formats.size());
				for (uint32_t i = 0; i < info.input_primitive_formats.size(); ++i)
				{
					attributeDescriptions.emplace_back(VkVertexInputAttributeDescription
						{
							.location = i,
							.binding = 0,
							.format = static_cast<VkFormat>(info.input_primitive_formats[i]),
							.offset = static_cast<uint32_t>(info.input_primitive_offsets[i]),
						}
					);
				}
			}
			VkPipelineVertexInputStateCreateInfo inputCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount = 1,
				.pVertexBindingDescriptions = &bindingDescriptions,
				.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
				.pVertexAttributeDescriptions = attributeDescriptions.data(),
			};
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				.primitiveRestartEnable = VK_FALSE,
			};

			std::vector<VkDynamicState> dynamicStates{
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR,
			};
			VkPipelineDynamicStateCreateInfo dynamic_create_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
				.pDynamicStates = dynamicStates.data(),
			};

			auto descriptorSize = static_cast<uint32_t>(frames.size());
			VkDescriptorPool descriptorPool;
			{
				std::vector<VkDescriptorPoolSize> poolSizes{
					{
					.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.descriptorCount = 2 * descriptorSize,
					},
					{
					.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 2 * descriptorSize,
					}
				};
				VkDescriptorPoolCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
					.maxSets = 2 * descriptorSize,
					.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
					.pPoolSizes = poolSizes.data(),
				};

				auto result = vkCreateDescriptorPool(logicDevice, &createInfo, nullptr, &descriptorPool);

				switch (result)
				{
				case VK_SUCCESS: break;
				default:
					const char* message;
					GET_VULKAN_ERROR_STRING(result, message,
						"Could not create a brush' descriptor pool on the GPU: ")
						throw std::runtime_error(message);
				}

				vulkan_descriptor_pool = unique_deleter_ptr<vulkan_handle::descriptor_pool_t>(from_vulkan(descriptorPool),
					[logicDevice](vulkan_handle::descriptor_pool p)
					{
						vkDeviceWaitIdle(logicDevice);
						vkDestroyDescriptorPool(logicDevice, to_vulkan(p), nullptr);
					}
				);
			}
			std::vector<VkDescriptorSet> descriptorSets;
			{
				std::vector<VkDescriptorSetLayout> descriptorLayouts(descriptorSize, descriptorSetLayout);

				VkDescriptorSetAllocateInfo allocateInfo{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
					.descriptorPool = descriptorPool,
					.descriptorSetCount = descriptorSize,
					.pSetLayouts = descriptorLayouts.data(),
				};

				descriptorSets.resize(descriptorSize);
				auto result = vkAllocateDescriptorSets(logicDevice, &allocateInfo, descriptorSets.data());

				switch (result)
				{
				case VK_SUCCESS: break;
				default:
					const char* message;
					GET_VULKAN_ERROR_STRING(result, message,
						"Could not create a brush' descriptor set on the GPU: ")
						throw std::runtime_error(message);
				}

				vulkan_descriptor_set.resize(descriptorSize);
				for (uint32_t i = 0; i < descriptorSize; i++)
				{
					vulkan_descriptor_set[i] = from_vulkan(descriptorSets[i]);
				}
			}

			uint32_t width, height;
			{
				auto size = window.pixel_size();
				width = static_cast<uint32_t>(size.x());
				height = static_cast<uint32_t>(size.y());
			}
			VkViewport viewport{
				.x = 0.f,
				.y = 0.f,
				.width = static_cast<float>(width),
				.height = static_cast<float>(height),
				.minDepth = 0.f,
				.maxDepth = 1.f,
			};
			VkRect2D scissor{
				.offset = {0, 0},
				.extent = {
					.width = width,
					.height = height,
				},
			};
			VkPipelineViewportStateCreateInfo viewportCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				.viewportCount = 1,
				.pViewports = &viewport,
				.scissorCount = 1,
				.pScissors = &scissor,
			};

			VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
				.depthClampEnable = VK_FALSE,
				.rasterizerDiscardEnable = VK_FALSE,
				.polygonMode = VK_POLYGON_MODE_FILL,
				.cullMode = VK_CULL_MODE_BACK_BIT,
				.frontFace = VK_FRONT_FACE_CLOCKWISE,
				.depthBiasEnable = VK_FALSE,
				.lineWidth = 1.f,
			};

			VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
				.sampleShadingEnable = VK_FALSE,
			};

			VkPipelineColorBlendAttachmentState blendState{
				.blendEnable = VK_FALSE,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			};
			VkPipelineColorBlendStateCreateInfo blendCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.attachmentCount = 1,
				.pAttachments = &blendState,
			};

			VkPipeline pipeline;
			{
				VkGraphicsPipelineCreateInfo createInfo{
					.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.stageCount = static_cast<uint32_t>(stageCreateInfo.size()),
					.pStages = stageCreateInfo.data(),
					.pVertexInputState = &inputCreateInfo,
					.pInputAssemblyState = &inputAssemblyCreateInfo,
					.pViewportState = &viewportCreateInfo,
					.pRasterizationState = &rasterizationCreateInfo,
					.pMultisampleState = &multisampleCreateInfo,
					.pDepthStencilState = nullptr,
					.pColorBlendState = &blendCreateInfo,
					.pDynamicState = &dynamic_create_info,
					.layout = pipelineLayout,
					.renderPass = renderpass,
					.subpass = 0,
					.basePipelineHandle = nullptr,
					.basePipelineIndex = -1,
				};

				auto result = vkCreateGraphicsPipelines(logicDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);

				switch (result)
				{
				case VK_SUCCESS: break;
				default:
					const char* message;
					GET_VULKAN_ERROR_STRING(result, message,
						"Could not create a brush' pipeline on the GPU: ")
						throw std::runtime_error(message);
				}

				vulkan_pipeline = unique_deleter_ptr<vulkan_handle::pipeline_t>(from_vulkan(pipeline),
					[logicDevice](vulkan_handle::pipeline p)
					{
						vkDeviceWaitIdle(logicDevice);
						vkDestroyPipeline(logicDevice, to_vulkan(p), nullptr);
					}
				);
			}

			vulkan_frame_buffer.reserve(frames.size());
			for (std::size_t frame_iterator = 0; frame_iterator < frames.size(); ++frame_iterator)
			{
				VkImageView swapchainImage = to_vulkan(frames[frame_iterator].image());

				VkFramebufferCreateInfo create_info{
					.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
					.renderPass = renderpass,
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
						"Could not create a brush' frame buffer on the GPU: ")
						throw std::runtime_error(message);
				}

				vulkan_frame_buffer.push_back(unique_deleter_ptr<vulkan_handle::frame_buffer_t>(from_vulkan(framebuffer),
					[logicDevice](vulkan_handle::frame_buffer b)
					{
						vkDeviceWaitIdle(logicDevice);
						vkDestroyFramebuffer(logicDevice, to_vulkan(b), nullptr);
					}
				));
			}
		}
	}
}
