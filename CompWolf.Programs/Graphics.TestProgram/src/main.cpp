#include <iostream>
#include <chrono>
#include <cmath>

#include <simple_drawables>
#include <vulkan_graphics>

struct compwolf::default_implementation : compwolf::vulkan_types {};
using graphics_types = compwolf::default_implementation;

static void debug_callback(std::string_view s)
{
	std::cout << s;
}

int main()
{
	graphics_types::environment environment(compwolf::vulkan::vulkan_graphics_environment_settings{
		{
			.program_name = "Hello Program!",
			.program_version = { 2, 5, 8 }, // Random numbers selected for testing
		},
		&debug_callback,
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

	compwolf::simple_brush<> brush(camera);
	compwolf::simple_square<> square(camera, brush
		, compwolf::simple_transform_data
		{
			.position = { .0f, .0f },
			.scale = { .25f, .25f },
		}
		, {.75f, .125f, .5f}
	);


	auto clock = std::chrono::high_resolution_clock();
	auto start_time = clock.now();
	double elapsed_time = 0.;
	double delta_time = 0.;

	int frames_since_last_report = 0;
	double next_report_time = 1.;

	while (window.running())
	{
		window.update_image();
		environment.update();

		if (environment.inputs().state_for('w').down()) square.transform().data()[0].position.y() -= static_cast<float>(delta_time);
		if (environment.inputs().state_for('a').down()) square.transform().data()[0].position.x() -= static_cast<float>(delta_time);
		if (environment.inputs().state_for('s').down()) square.transform().data()[0].position.y() += static_cast<float>(delta_time);
		if (environment.inputs().state_for('d').down()) square.transform().data()[0].position.x() += static_cast<float>(delta_time);

		// get new time
		auto old_time = elapsed_time;
		elapsed_time = std::chrono::duration<double>(clock.now() - start_time).count();
		delta_time = elapsed_time - old_time;

		// report framerate
		++frames_since_last_report;
		if (elapsed_time >= next_report_time) [[unlikely]]
		{
			auto framerate = frames_since_last_report / (elapsed_time - next_report_time + 1);
			std::cout << "framerate: " << framerate << std::endl;
			next_report_time = elapsed_time + 1;
			frames_since_last_report = 0;
		}
	}

	std::cout << "\nEnding...\n";
}
