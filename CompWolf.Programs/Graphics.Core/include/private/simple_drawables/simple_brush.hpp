#ifndef COMPWOLF_GRAPHICS_SIMPLE_BRUSH
#define COMPWOLF_GRAPHICS_SIMPLE_BRUSH

#include <graphics_environments>
#include <dimensions>
#include "simple_vertex_shader.hpp"
#include "single_color_pixel_shader.hpp"

namespace compwolf
{
	/** A simple brush.
	 * It takes, as its input, 2D-position as its vertices' position.
	 * Its fields are:
	 * * The object's position, as a [[float2]].
	 * * The object's RGB color, as a [[float3]].
	 * @typeparam Implementation The implementation of [[CompWolf.Graphics]] to use.
	 */
	template <ImplementationType Implementation = default_implementation>
	class simple_brush : public Implementation::template brush<
		typename simple_vertex_shader<Implementation>::shader_type,
		typename single_color_pixel_shader<Implementation>::shader_type
	>
	{
		using super = typename Implementation::template brush<
			typename simple_vertex_shader<Implementation>::shader_type,
			typename single_color_pixel_shader<Implementation>::shader_type
		>;

	public: // constructors
		/** Constructs an invalid [[simple_brush]].
		 * Using this brush is undefined behaviour.
		 * @overload
		 */
		simple_brush() = default;
		simple_brush(simple_brush&&) = default;
		auto operator=(simple_brush&&) -> simple_brush& = default;

		/** Creates a brush on the given gpu. */
		simple_brush(Implementation::gpu& gpu) : super(
			simple_vertex_shader<Implementation>::get(gpu),
			single_color_pixel_shader<Implementation>::get(gpu))
		{
		}
		/** Creates a brush on the given camera's gpu. */
		simple_brush(Implementation::camera& camera)
			: simple_brush(camera.gpu())
		{ }
	};
}

#endif // ! COMPWOLF_GRAPHICS_SIMPLE_BRUSH
