#ifndef COMPWOLF_GRAPHICS_SIMPLE_VERTEX_SHADER
#define COMPWOLF_GRAPHICS_SIMPLE_VERTEX_SHADER

#include <graphics_environments>
#include <shaders>
#include <gpu_structs>
#include <dimensions>

namespace compwolf
{
	namespace internal
	{
		constexpr const char simple_vertex_shader_path[] = "resources/CompWolf.Graphics.simple_vertex_shader.spv";
	}

	/** [[simple_vertex_shader]]'s field. */
	struct simple_transform_data
	{
		float2 position;
		float2 scale;

		constexpr auto operator==(const simple_transform_data& other) const noexcept -> bool
		{
			return position == other.position
				&& scale == other.scale;
		}
		constexpr auto operator!=(const simple_transform_data& other) const noexcept -> bool
		{
			return !this->operator==(other);
		}
	};
	template<> struct compwolf::gpu_struct_info<simple_transform_data> : public compwolf::new_gpu_struct_info<
		compwolf::type_value_pair<compwolf::float2, offsetof(simple_transform_data, position)>,
		compwolf::type_value_pair<compwolf::float2, offsetof(simple_transform_data, scale)>
	> { };

	/** A simple input shader for drawing objects.
	 * It takes, as its input, 2D-position as its vertices' position.
	 * It also has a 2D-position as a field, denoting the objects position.
	 * @typeparam Implementation The implementation of [[CompWolf.Graphics]] to use.
	 */
	template <ImplementationType Implementation = default_implementation>
	using simple_vertex_shader = static_shader<internal::simple_vertex_shader_path,
		typename Implementation::template shader<
			float2, float4, type_value_pair<simple_transform_data, 0>
		>
	>;
}

#endif // ! COMPWOLF_GRAPHICS_SIMPLE_VERTEX_SHADER
