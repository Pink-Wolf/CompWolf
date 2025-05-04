#ifndef COMPWOLF_GRAPHICS_SINGLE_COLOR_PIXEL_SHADER
#define COMPWOLF_GRAPHICS_SINGLE_COLOR_PIXEL_SHADER

#include <graphics_environments>
#include <shaders>
#include <dimensions>

namespace compwolf
{
	namespace internal
	{
		constexpr const char single_color_pixel_shader_path[] = "resources/CompWolf.Graphics.single_color_pixel_shader.spv";
	}
	/** A simple pixel shader for drawing objects.
	 * It has a RGB-color as a field, denoting the color of the object.
	 * @typeparam Implementation The implementation of [[CompWolf.Graphics]] to use.
	 */
	template <ImplementationType Implementation = default_implementation>
	using single_color_pixel_shader = static_shader<internal::single_color_pixel_shader_path,
		typename Implementation::template shader<
		float4, pixel_output_type, type_value_pair<float3, 4>
		>
	>;
}

#endif // ! COMPWOLF_GRAPHICS_SINGLE_COLOR_PIXEL_SHADER
