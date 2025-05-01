#ifndef COMPWOLF_GRAPHICS_VULKAN_BRUSH
#define COMPWOLF_GRAPHICS_VULKAN_BRUSH

#include <vulkan_graphics_environments>
#include <drawables>
#include <gpu_structs>
#include "vulkan_brush_internal.hpp"
#include <compwolf_type_traits>
#include <vulkan_gpu_structs>
#include <map>
#include <utility>

namespace compwolf::vulkan
{
	namespace internal
	{
		/** @hidden */
		template <typename T>
		using vulkan_brush_get_from_pair = T;

		/** @hidden */
		template <typename TypeList>
		struct is_in
		{
			template <typename T>
			struct transformer
			{
				static constexpr bool value = TypeList::template has<T>;
			};
		};
	}

	/** A Vulkan-implementation of [[brush]].
	 * @typeparam GraphicsEnvironmentType The type of [[graphics_environment]] that this buffer works with.
	 * @typeparam InputShaderType The type of vertex shader used by the brush.
	 * That is, when drawing an object, the shader used to modify the object's elements/vertices.
	 * @typeparam PixelShaderType The type of fragment shader used by the brush.
	 * That is, when drawing an object, the shader used to determine the individual pixel colors of the drawing.
	 * @see brush
	 * @see vulkan_graphics_environment
	 */
	template <typename InputShaderType, typename PixelShaderType>
	class vulkan_brush
		: public brush<vulkan_graphics_environment, InputShaderType, PixelShaderType>
	{
		using super = brush<vulkan_graphics_environment, InputShaderType, PixelShaderType>;

	private:
		static constexpr internal::vulkan_brush_info _internal_info
		{
			.input_primitive_formats
				= gpu_struct_info<typename super::input_shader_type::input_type>::primitives
				:: template transform<internal::vulkan_brush_get_from_pair>
				:: template transform_to_value<
					get_vulkan_format,
					std::vector<vulkan_handle::format>
				>,
			.input_primitive_offsets
				= gpu_struct_info<typename super::input_shader_type::input_type>::primitives
				:: template transform_to_value<
					internal::vulkan_brush_get_from_pair,
					std::vector<std::size_t>
				>,
			.input_stride
				= sizeof(typename super::input_shader_type::input_type),

			.field_indices
				= &super::field_positions(),
			.field_is_input_field
				= super::field_types::template transform_to_value<
					internal::template is_in<typename super::input_shader_type::field_types>::transformer,
					std::vector<bool>
				>,
			.field_is_pixel_field
				= super::field_types::template transform_to_value<
					internal::template is_in<typename super::pixel_shader_type::field_types>::transformer,
					std::vector<bool>
				>,
		};
		internal::vulkan_brush_internal _internal;
		mutable std::map<vulkan_window*, internal::vulkan_window_brush> _window_data;

	private: // vulkan-specific
		/** Gets the data for the given window.
		 * This creates the data if it is not already created.
		 */
		auto get_window_data(vulkan_window& window) const -> internal::vulkan_window_brush
		{
			auto i = _window_data.find(&window);
			if (i != _window_data.end()) return i->second;
			return *_window_data.emplace(window
				, _internal_info, super::input_shader().vulkan_shader_module(), super::pixel_shader().vulkan_shader_module()
				, _internal.vulkan_descriptor_set_layout, _internal.vulkan_pipeline_layout
			).first;
		}
	public:
		/** Returns the [[vulkan_handle::pipeline_layout]] of the pipeline that the brush represents. */
		auto vulkan_pipeline_layout() const noexcept -> vulkan_handle::pipeline_layout { return _internal.vulkan_pipeline_layout.get(); }

		/** Returns the [[vulkan_handle::descriptor_set_layout]] of the pipeline that the brush represents. */
		auto vulkan_descriptor_set_layout() const noexcept -> vulkan_handle::descriptor_set_layout { return _internal.vulkan_descriptor_set_layout.get(); }

		/** Returns the [[vulkan_handle::descriptor_pool]] of the pipeline that the brush represents.
		 * @param window the [[vulkan_window]] that the descriptor pool is for.
		 */
		auto vulkan_descriptor_pool(vulkan_window& window) const -> vulkan_handle::descriptor_pool
			{ return get_window_data(window).descriptor_set_layout.get(); }
		/** Returns the [[vulkan_handle::descriptor_set]] of the pipeline that the brush represents.
		 * @param window the [[vulkan_window]] that the descriptor set is for.
		 */
		auto vulkan_descriptor_set(vulkan_window& window) const -> const std::vector<vulkan_handle::descriptor_set>&
			{ return get_window_data(window).vulkan_descriptor_set; }
		
		/** Returns the [[vulkan_handle::pipeline]] that the brush represents.
		 * @param window the [[vulkan_window]] that the pipeline is for.
		 */
		auto vulkan_pipeline(vulkan_window& window) const -> vulkan_handle::pipeline
			{ return get_window_data(window).vulkan_pipeline.get(); }
		
		/** Returns the [[vulkan_handle::frame_buffer]] that the brush represents.
		 * @param window the [[vulkan_window]] that the buffer is for.
		 * @param frame_index the index of the frame that the buffer is for.
		 */
		auto vulkan_frame_buffer(vulkan_window& window, std::size_t frame_index) const -> vulkan_handle::frame_buffer
			{ return get_window_data(window).vulkan_frame_buffer.at(frame_index).get(); }

	public: // constructors
		/** Constructs an invalid [[vulkan_brush]].
		 * Using this brush is undefined behaviour.
		 * @overload
		 */
		vulkan_brush() = default;
		vulkan_brush(vulkan_brush&&) = default;
		auto operator=(vulkan_brush&&) -> vulkan_brush& = default;

		/** Creates a brush on the given gpu. */
		vulkan_brush(super::input_shader_type& input_shader, super::pixel_shader_type& pixel_shader)
			: super(input_shader, pixel_shader)
			, _internal(super::gpu(), _internal_info)
		{
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_BRUSH
