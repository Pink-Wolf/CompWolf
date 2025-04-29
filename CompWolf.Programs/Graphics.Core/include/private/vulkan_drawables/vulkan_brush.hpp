#ifndef COMPWOLF_GRAPHICS_VULKAN_BRUSH
#define COMPWOLF_GRAPHICS_VULKAN_BRUSH

#include <vulkan_graphics_environments>
#include <drawables>
#include <gpu_structs>
#include "vulkan_brush_internal.hpp"
#include <compwolf_type_traits>
#include <vulkan_gpu_structs>

namespace compwolf::vulkan
{
	namespace internal
	{
		template <typename T>
		using vulkan_brush_get_from_pair = T;

		template <typename TypeList>
		struct is_in
		{
			template <typename T>
			struct transformer
			{
				static constexpr bool value = TypeList::template contains<T>;
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
		internal::vulkan_brush_internal _internal;

	public: // vulkan-specific
		/** Returns the [[vulkan_handle::pipeline_layout]] of the pipeline that the brush represents. */
		auto vulkan_pipeline_layout() const noexcept -> vulkan_handle::pipeline_layout { return _internal.pipeline_layout.get(); }

	public: // constructors
		/** Constructs an invalid [[vulkan_brush]].
		 * Using this brush is undefined behaviour.
		 * @overload
		 */
		vulkan_brush() = default;
		vulkan_brush(vulkan_brush&&) = default;
		auto operator=(vulkan_brush&&) -> vulkan_brush& = default;

		/** Creates a brush on the given gpu. */
		vulkan_brush(vulkan_gpu_connection& gpu
			, super::input_shader_type& input_shader, super::pixel_shader_type& pixel_shader)
			: super(gpu)
			, _internal(gpu, internal::vulkan_brush_info
				{
					.input_primitive_formats
						= gpu_struct_info<typename super::input_shader_type::input_type>::primitives
						:: template transform_to_value<
							get_vulkan_format,
							std::vector<vulkan_handle::format>
						>,
					.input_primitive_offsets
						= gpu_struct_info<typename super::input_shader_type::input_type>::primitives
						::template transform_to_value<
							internal::vulkan_brush_get_from_pair,
							std::vector<std::size_t>
						>,
					.input_stride
						= sizeof(typename super::input_shader_type::input_type),

					.field_primitive_indices
						= super::field_types
						::template transform_to_value<
							internal::vulkan_brush_get_from_pair,
							std::vector<std::size_t>
						>,
					.field_is_input_field
						= super::input_shader_type::template transform_to_value<
							internal::template is_in<super::field_types>::transformer,
							std::vector<bool>
						>,
					.field_is_pixel_field
						= super::pixel_shader_type::template transform_to_value<
							internal::template is_in<super::field_types>::transformer,
							std::vector<bool>
						>,

					.input_shader = input_shader.vulkan_shader_module(),
					.pixel_shader = pixel_shader.vulkan_shader_module(),
				})
		{
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_BRUSH
