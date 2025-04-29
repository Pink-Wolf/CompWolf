#ifndef COMPWOLF_GRAPHICS_VULKAN_BRUSH_INTERNAL
#define COMPWOLF_GRAPHICS_VULKAN_BRUSH_INTERNAL

#include <vulkan_graphics_environments>
#include <vulkan_shaders>
#include <unique_deleter_ptr>
#include <utility>
#include <cstdint>
#include <vector>

namespace compwolf::vulkan::internal
{
	/** @hidden */
	struct vulkan_brush_info
	{
		const std::vector<vulkan_handle::format> input_primitive_formats;
		const std::vector<std::size_t> input_primitive_offsets;
		std::size_t input_stride;

		std::vector<std::size_t> field_primitive_indices;
		std::vector<bool> field_is_input_field;
		std::vector<bool> field_is_pixel_field;

		vulkan_handle::shader input_shader;
		vulkan_handle::shader pixel_shader;
	};

	/** @hidden */
	class vulkan_brush_internal
	{
	public:
		unique_deleter_ptr<vulkan_handle::pipeline_layout> vulkan_shader{};

	public: // constructors
		/** Constructs an invalid [[vulkan_brush_internal]].
		 * Using this brush is undefined behaviour.
		 * @overload
		 */
		vulkan_brush_internal() = default;
		vulkan_brush_internal(vulkan_brush_internal&&) = default;
		auto operator=(vulkan_brush_internal&&) -> vulkan_brush_internal& = default;

		/** Creates a brush on the given gpu. */
		vulkan_brush_internal(vulkan_gpu_connection& gpu
			, vulkan_brush_info&& info
		);
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_BRUSH_INTERNAL
