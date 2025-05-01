#include <iostream>
#include <chrono>
#include <cmath>

#include <vulkan_graphics_environments>
#include <vulkan_windows>
#include <vulkan_shaders>
#include <vulkan_gpu_buffers>
#include <vulkan_drawables>

static void debug_callback(std::string_view s)
{
	std::cout << s;
}

constexpr const char input_shader_path[] = "resources/vertex.spv";
constexpr const char pixel_shader_path[] = "resources/pixel.spv";

int main()
{
	{
		compwolf::vulkan::vulkan_graphics_environment_settings es{
			.internal_debug_callback = &debug_callback
		};
		compwolf::vulkan::vulkan_graphics_environment e(es);

		{
			compwolf::window_settings ws{
				.name = "Window1",
				.pixel_size = {640, 480},
			};
			compwolf::vulkan::vulkan_window w(e, ws);

			{
				compwolf::vulkan::vulkan_camera camera(w, compwolf::window_camera_settings
					{
						.background_color = {.25f, 0.25f, .5f}
					}
				);

				{
					using input_shader_type = compwolf::vulkan::vulkan_shader<
						compwolf::float2, compwolf::float4
					>;
					input_shader_type input_shader(w.gpu(), compwolf::shader_code_from_file(input_shader_path));

					using pixel_shader_type = compwolf::vulkan::vulkan_shader<
						compwolf::float4, compwolf::pixel_output_type
						, compwolf::type_value_pair<compwolf::float3, 4>
					>;
					pixel_shader_type pixel_shader(w.gpu(), compwolf::shader_code_from_file(pixel_shader_path));

					{
						using brush_type = compwolf::vulkan::vulkan_brush<input_shader_type, pixel_shader_type>;
						brush_type brush(input_shader, pixel_shader);

						{
							using vertex_buffer_type = compwolf::vulkan::vulkan_gpu_buffer<compwolf::gpu_buffer_usage::input, compwolf::float2>;
							vertex_buffer_type vertex_buffer(w.gpu(), 4);
							{
								auto data = vertex_buffer.data();
								data[0] = { -.5f, -.5f };
								data[1] = { -.5f, +.5f };
								data[2] = { +.5f, -.5f };
								data[3] = { +.5f, +.5f };
							}

							using index_buffer_type = compwolf::vulkan::vulkan_gpu_buffer<compwolf::gpu_buffer_usage::input_index, compwolf::shader_int>;
							index_buffer_type index_buffer(w.gpu(), 6);
							{
								auto data = index_buffer.data();
								data[0] = 0;
								data[1] = 2;
								data[2] = 1;
								data[3] = 1;
								data[4] = 2;
								data[5] = 3;
							}

							using color_buffer_type = compwolf::vulkan::vulkan_gpu_buffer<compwolf::gpu_buffer_usage::field, compwolf::float3>;
							color_buffer_type color_buffer(w.gpu(), 1);
							color_buffer.data()[0] = { .75f, .125f, .5f };

							{
								compwolf::vulkan::vulkan_drawable<brush_type> square(camera, brush
									, vertex_buffer, index_buffer
									, color_buffer
								);

								{
									auto event_key = e.inputs().char_newly_down().subscribe([](const compwolf::input_key_state& key)
										{
											std::cout << key.character();
										}
									);

									auto clock = std::chrono::high_resolution_clock();
									auto start_time = clock.now();
									int frame_count = 0;
									int frames_per_report = 60;

									while (w.running())
									{
										w.update_image();
										e.update();

										++frame_count;
										if (frame_count >= frames_per_report) [[unlikely]]
										{
											auto elapsed_time = std::chrono::duration<double>(clock.now() - start_time).count();
											auto framerate = frame_count / elapsed_time;
											std::cout << "framerate: " << framerate << std::endl;

											frames_per_report = std::ceil(framerate);
											frame_count = 0;
											start_time = clock.now();
										}
									}

									std::cout << "\nEnding...\n";
								}
								std::cout << "destroying drawable\n";
							}
							std::cout << "destroying buffers\n";
						}
						std::cout << "destroying brush\n";
					}
					std::cout << "destroying shaders\n";
				}
				std::cout << "destroying camera\n";
			}
			std::cout << "destroying window\n";
		}
		std::cout << "destroying environment\n";
	}
	std::cout << "everything is destroyed\n";
}
