#include <private/vulkan_drawables/vulkan_drawable.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace compwolf::vulkan::internal
{
	void drawable_draw_code(const vulkan_draw_code_parameters& args
		, vulkan_handle::pipeline pipeline
		, vulkan_handle::pipeline_layout pipeline_layout
		, shader_int vertex_index_count
		, vulkan_handle::buffer vertex_buffer
		, vulkan_handle::buffer vertex_index_buffer
		, const vulkan_handle::buffer* field_buffers
		, const std::vector<shader_int>& field_indices
		, const std::vector<vulkan_handle::descriptor_set>& descriptor_sets
	)
	{
		auto& gpu = args.window->gpu();
		auto command = to_vulkan(args.command);
		auto logicDevice = to_vulkan(gpu.vulkan_device());
		auto vkPipeline = to_vulkan(pipeline);
		auto vkPipelineLayout = to_vulkan(pipeline_layout);
		auto descriptorSet = to_vulkan(descriptor_sets[args.frame_index]);

		{
			vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

			uint32_t width, height;
			{
				auto size = args.window->pixel_size();
				width = static_cast<uint32_t>(size.x());
				height = static_cast<uint32_t>(size.y());
			}
			VkViewport viewport{
				.x = .0f,
				.y = .0f,
				.width = static_cast<float>(width),
				.height = static_cast<float>(height),
				.minDepth = .0f,
				.maxDepth = 1.f,
			};
			vkCmdSetViewport(command, 0, 1, &viewport);

			VkRect2D renderArea{
				.offset = {0, 0},
				.extent = {
					.width = width,
					.height = height,
				},
			};
			vkCmdSetScissor(command, 0, 1, &renderArea);
		}
		{
			auto vkBuffer = to_vulkan(vertex_buffer);
			static VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(command, 0, 1, &vkBuffer, offsets);
		}
		{
			auto indexBuffer = to_vulkan(vertex_index_buffer);

			vkCmdBindIndexBuffer(command, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
		for (size_t i = 0; i < field_indices.size(); ++i)
		{
			VkDescriptorBufferInfo bufferInfo{
				.buffer = to_vulkan(field_buffers[i]),
				.offset = 0,
				.range = VK_WHOLE_SIZE,
			};
			VkWriteDescriptorSet writer{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = descriptorSet,
				.dstBinding = field_indices[i],
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &bufferInfo,
			};

			vkUpdateDescriptorSets(logicDevice, 1, &writer, 0, nullptr);

			vkCmdBindDescriptorSets(command
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, vkPipelineLayout
				, 0
				, 1
				, &descriptorSet
				, 0
				, nullptr
			);
		}

		vkCmdDrawIndexed(command, vertex_index_count, 1, 0, 0, 0);
	}
}
