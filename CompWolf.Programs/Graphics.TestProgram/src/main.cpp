#include <iostream>
#include <vulkan_graphics_environments>
#include <vulkan_windows>
#include <chrono>

static void debug_callback(std::string_view s)
{
	std::cout << s;
}

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
						.background_color = {1, 0, .5f}
					}
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
				std::cout << "destroying camera\n";
			}
			std::cout << "destroying window\n";
		}
		std::cout << "destroying environment\n";
	}
	std::cout << "everything is destroyed\n";
}
