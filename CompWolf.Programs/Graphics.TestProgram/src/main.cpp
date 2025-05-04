#include <iostream>
#include <chrono>
#include <cmath>

#include <simple_drawables>
#include <vulkan_graphics>

using graphics_types = compwolf::vulkan_types;

static void debug_callback(std::string_view s)
{
	std::cout << s;
}

int main()
{
	graphics_types::environment environment(compwolf::vulkan::vulkan_graphics_environment_settings{
		.internal_debug_callback = &debug_callback
	});

	graphics_types::window window(environment, compwolf::window_settings{
		.name = "Hello Window!",
		.pixel_size = {640, 640},
	});

	graphics_types::camera camera(window, compwolf::window_camera_settings
		{
			.background_color = {.25f, 0.25f, .5f}
		}
	);

	compwolf::simple_brush<graphics_types> brush(camera);
	compwolf::simple_square<graphics_types> square(camera, brush
		, compwolf::simple_transform_data
		{
			.position = { .0f, .0f },
			.scale = { .25f, .25f },
		}
		, {.75f, .125f, .5f}
	);

	auto clock = std::chrono::high_resolution_clock();
	auto start_time = clock.now();
	int frame_count = 0;
	int frames_per_report = 60;

	while (window.running())
	{
		window.update_image();
		environment.update();

		if (environment.inputs().state_for('w').down()) square.transform().data()[0].position.y() -= .0005f;
		if (environment.inputs().state_for('a').down()) square.transform().data()[0].position.x() -= .0005f;
		if (environment.inputs().state_for('s').down()) square.transform().data()[0].position.y() += .0005f;
		if (environment.inputs().state_for('d').down()) square.transform().data()[0].position.x() += .0005f;

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
