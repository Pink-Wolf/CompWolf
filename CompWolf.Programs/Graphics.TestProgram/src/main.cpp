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

constexpr const char input_shader_path[] = "resources/vertex.spv";
using input_shader_type = compwolf::static_shader<input_shader_path, compwolf::vulkan::vulkan_shader<
	compwolf::float2, compwolf::float4
>>;
constexpr const char pixel_shader_path[] = "resources/pixel.spv";
using pixel_shader_type = compwolf::static_shader<pixel_shader_path, compwolf::vulkan::vulkan_shader<
	compwolf::float4, compwolf::pixel_output_type,
	compwolf::type_value_pair<compwolf::float3, 4>
>>;

int main()
{
	compwolf::vulkan::vulkan_graphics_environment_settings environment_settings{
		.internal_debug_callback = &debug_callback
	};
	graphics_types::environment environment(environment_settings);

	graphics_types::window window(environment, compwolf::window_settings{
				.name = "Hello Window!",
				.pixel_size = {640, 480},
	});
	auto& gpu = window.gpu();

	graphics_types::camera camera(window, compwolf::window_camera_settings
		{
			.background_color = {.25f, 0.25f, .5f}
		}
	);

	compwolf::simple_shape<graphics_types>::vertex_buffer_type
		vertices(window.gpu(), 4);
	{
		auto data = vertices.data();
		data[0] = { -.5f, -.5f };
		data[1] = { -.5f, +.5f };
		data[2] = { +.5f, -.5f };
		data[3] = { +.5f, +.5f };
	}

	compwolf::simple_shape<graphics_types>::vertex_index_buffer_type
		indices(window.gpu(), 6);
	{
		auto data = indices.data();
		data[0] = 0;
		data[1] = 2;
		data[2] = 1;
		data[3] = 1;
		data[4] = 2;
		data[5] = 3;
	}

	compwolf::simple_brush<graphics_types> brush(camera);
	compwolf::simple_shape<graphics_types> square(camera, brush, vertices, indices
		, { .25f, .25f }, { .75f, .125f, .5f }
	);


	auto event_key = environment.inputs().char_newly_down().subscribe([&square](const compwolf::input_key_state& key)
		{
			if (key.lowercase_character() == 'w') square.position().data()[0].y() -= .1f;
			if (key.lowercase_character() == 'a') square.position().data()[0].x() -= .1f;
			if (key.lowercase_character() == 's') square.position().data()[0].y() += .1f;
			if (key.lowercase_character() == 'd') square.position().data()[0].x() += .1f;
		}
	);

	auto clock = std::chrono::high_resolution_clock();
	auto start_time = clock.now();
	int frame_count = 0;
	int frames_per_report = 60;

	while (window.running())
	{
		window.update_image();
		environment.update();

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
