#include "private/vulkan_windows/vulkan_camera.hpp"
#include "compwolf_vulkan.hpp"

#include "private/vulkan_windows/vulkan_window.hpp"
#include <stdexcept>

namespace compwolf::vulkan
{
	vulkan_camera::vulkan_camera(vulkan_window& window_in, window_camera_settings settings)
		: window_camera(window_in, settings)
		, _draw_programs(window_in.swapchain().frames().size())
	{
		// This is seemingly needed to get around compiler bug: https://stackoverflow.com/questions/29459040/why-copy-constructor-is-called-instead-of-move-constructor
		_drawing_key.~event_key();
		new(&_drawing_key)event_key(window().drawing().subscribe(
			[this](const window_draw_parameters& draw_args)
			{
				if (_draw_programs.empty()) _draw_programs.resize(draw_args.target_frame_index);
				auto& current_program = _draw_programs[draw_args.target_frame_index];

				if (!current_program)
				{
					// This is seemingly needed to get around compiler bug: https://stackoverflow.com/questions/29459040/why-copy-constructor-is-called-instead-of-move-constructor
					current_program.~vulkan_gpu_program();
					new(&current_program)vulkan_gpu_program(
						draw_args.target_frame->draw_manager(),
						[this, draw_args](const vulkan_code_parameters& code_args)
						{
							auto commandBuffer = to_vulkan(code_args.command);

							VkClearValue clearColor = {
								{{
									background_color().x(),
									background_color().y(),
									background_color().z()
								}}
							};

							uint32_t width, height;
							{
								auto size = draw_args.target_window->pixel_size();
								width = static_cast<uint32_t>(size.x());
								height = static_cast<uint32_t>(size.y());
							}

							VkRenderPassBeginInfo renderpassInfo{
								.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
								.renderPass = to_vulkan(draw_args.target_window->surface().vulkan_render_pass()),
								.framebuffer = to_vulkan(draw_args.target_frame->frame_buffer()),
								.renderArea = {
									.offset = {0, 0},
									.extent = {
										.width = width,
										.height = height,
									},
								},
								.clearValueCount = 1,
								.pClearValues = &clearColor,
							};

							vkCmdBeginRenderPass(commandBuffer, &renderpassInfo, VK_SUBPASS_CONTENTS_INLINE);

							vulkan_draw_code_parameters draw_code_args{
								code_args,
								draw_args.target_frame,
								draw_args.target_frame_index
							};
							_drawing_code.invoke(draw_code_args);

							vkCmdEndRenderPass(commandBuffer);
						}
					);
				}

				current_program.execute();
			}
		));
	}
}
