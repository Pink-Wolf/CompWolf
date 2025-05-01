#ifndef COMPWOLF_GRAPHICS_VULKAN_BRUSH_INTERNAL
#define COMPWOLF_GRAPHICS_VULKAN_BRUSH_INTERNAL

#include <vulkan_graphics_environments>
#include <vulkan_shaders>
#include <unique_deleter_ptr>
#include <utility>
#include <cstdint>
#include <vector>
#include <vulkan_windows>

namespace compwolf::vulkan::internal
{
	/** @hidden */
	struct vulkan_brush_info
	{
		const std::vector<vulkan_handle::format> input_primitive_formats;
		const std::vector<std::size_t> input_primitive_offsets;
		std::size_t input_stride;

		const std::vector<std::size_t>* field_indices;
		std::vector<bool> field_is_input_field;
		std::vector<bool> field_is_pixel_field;
	};

	/** @hidden */
	class vulkan_brush_internal
	{
	public:
		unique_deleter_ptr<vulkan_handle::descriptor_set_layout_t> vulkan_descriptor_set_layout{};
		unique_deleter_ptr<vulkan_handle::pipeline_layout_t> vulkan_pipeline_layout{};

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
			, vulkan_brush_info& info
		);
	};

	/** Brushes actually have to be made per window.
	 * @hidden
	 */
	class vulkan_window_brush
	{
	public:
		unique_deleter_ptr<vulkan_handle::descriptor_pool_t> vulkan_descriptor_pool{};
		/** Descriptor sets do not need to be cleaned up explicitly; they are cleaned up when the pool is cleaned up. */
		std::vector<vulkan_handle::descriptor_set> vulkan_descriptor_set{};

		unique_deleter_ptr<vulkan_handle::pipeline_t> vulkan_pipeline{};
		std::vector<unique_deleter_ptr<vulkan_handle::frame_buffer_t>> vulkan_frame_buffer{};

	public: // constructors
		/** Constructs an invalid [[vulkan_window_brush]].
		 * Using this brush is undefined behaviour.
		 * @overload
		 */
		vulkan_window_brush() = default;
		vulkan_window_brush(vulkan_window_brush&&) = default;
		auto operator=(vulkan_window_brush&&) -> vulkan_window_brush& = default;

		/** Creates a brush for the given window. */
		vulkan_window_brush(vulkan_window& window
			, vulkan_brush_info& info, vulkan_handle::shader input_shader, vulkan_handle::shader pixel_shader
			, vulkan_handle::descriptor_set_layout, vulkan_handle::pipeline_layout
		);
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_BRUSH_INTERNAL
