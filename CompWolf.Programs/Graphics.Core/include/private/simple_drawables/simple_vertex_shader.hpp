#ifndef COMPWOLF_GRAPHICS_SIMPLE_VERTEX_SHADER
#define COMPWOLF_GRAPHICS_SIMPLE_VERTEX_SHADER

#include <graphics_environments>
#include <shaders>
#include <dimensions>

namespace compwolf
{
	namespace internal
	{
		constexpr const char simple_vertex_shader_path[] = "resources/CompWolf.Graphics.simple_vertex_shader.spv";
	}
	/** A simple input shader for drawing objects.
	 * It takes, as its input, 2D-position as its vertices' position.
	 * It also has a 2D-position as a field, denoting the objects position.
	 * @typeparam Implementation The implementation of [[CompWolf.Graphics]] to use.
	 */
	template <ImplementationType Implementation>
	using simple_vertex_shader = static_shader<internal::simple_vertex_shader_path,
		typename Implementation::template shader<
			float2, float4, type_value_pair<float2, 0>
		>
	>;
}

#endif // ! COMPWOLF_GRAPHICS_SIMPLE_VERTEX_SHADER
