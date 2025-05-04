#ifndef COMPWOLF_GRAPHICS_VULKAN_GRAPHICS
#define COMPWOLF_GRAPHICS_VULKAN_GRAPHICS

#include <vulkan_graphics_environments>
#include <vulkan_windows>

#include <vulkan_gpu_structs>
#include <vulkan_gpu_buffers>
#include <vulkan_shaders>
#include <vulkan_programs>
#include <vulkan_drawables>

namespace compwolf
{
	struct vulkan_types
	{
		using environment = vulkan::vulkan_graphics_environment;
		using gpu = vulkan::vulkan_gpu_connection;

		using window = vulkan::vulkan_window;
		using camera = vulkan::vulkan_camera;

		/**
		 * @typeparam UsageType How the data is going to be used.
		 * @typeparam ValueType The type of data in the buffer.
		 */
		template <gpu_buffer_usage UsageType, typename ValueType>
		using buffer = vulkan::vulkan_gpu_buffer<UsageType, ValueType>;

		/**
		 * @typeparam InputType The type of element that the shader takes as input.
		 * @typeparam OutputType The type of element that the shader outputs.
		 * @typeparam FieldTypes The fields that the shader has.
		 * These must be [[type_value_pair]]s, denoting the type and position of the fields.
		 */
		template <typename InputType, typename OutputType, typename... FieldTypes>
		using shader = vulkan::vulkan_shader<InputType, OutputType, FieldTypes...>;

		using fence = vulkan::vulkan_gpu_fence;
		using program = vulkan::vulkan_gpu_program;

		/**
		 * @typeparam InputShaderType The type of vertex shader used by the brush.
		 * That is, when drawing an object, the shader used to modify the object's elements/vertices.
		 * @typeparam PixelShaderType The type of fragment shader used by the brush.
		 * That is, when drawing an object, the shader used to determine the individual pixel colors of the drawing.
		 */
		template <typename InputShaderType, typename PixelShaderType>
		using brush = vulkan::vulkan_brush<InputShaderType, PixelShaderType>;
		/**
		 * @typeparam BrushType The type of brush that can be used to draw this.
		 */
		template <typename BrushType>
		using drawable = vulkan::vulkan_drawable<BrushType>;
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_GRAPHICS
