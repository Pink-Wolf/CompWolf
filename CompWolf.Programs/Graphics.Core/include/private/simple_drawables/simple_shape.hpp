#ifndef COMPWOLF_GRAPHICS_SIMPLE_SHAPE
#define COMPWOLF_GRAPHICS_SIMPLE_SHAPE

#include <graphics_environments>
#include <dimensions>
#include "simple_brush.hpp"
#include <gpu_structs>
#include <gpu_buffers>

namespace compwolf
{
	/** A simple brush.
	 * It takes, as its input, 2D-position as its vertices' position.
	 * Its fields are:
	 * * The object's position, as a [[float2]].
	 * * The object's RGB color, as a [[float3]].
	 * @typeparam Implementation The implementation of [[CompWolf.Graphics]] to use.
	 */
	template <ImplementationType Implementation>
	class simple_shape
	{
	public:
		using vertex_buffer_type = typename Implementation::template buffer<
			gpu_buffer_usage::input,
			typename simple_vertex_shader<Implementation>::input_type
		>;
		using vertex_index_buffer_type = typename Implementation::template buffer<
			gpu_buffer_usage::input_index,
			shader_int
		>;

		using position_buffer_type = typename Implementation::template buffer<
			gpu_buffer_usage::field,
			float2
		>;
		using color_buffer_type = typename Implementation::template buffer<
			gpu_buffer_usage::field,
			float3
		>;

		using drawable_type = Implementation::template drawable<simple_brush<Implementation>>;

	private:
		vertex_buffer_type* _vertices;
		vertex_index_buffer_type* _indices;
		position_buffer_type _position;
		color_buffer_type _color;
		drawable_type _drawable;

	public:
		/** Returns the vertices making up the shape.
		 * @customoverload
		 */
		auto vertices() noexcept -> vertex_buffer_type& { return *_vertices; }
		/** Returns the vertices making up the shape. */
		auto vertices() const noexcept -> const vertex_buffer_type& { return *_vertices; }
		/** Returns the index of the vertices making up the shape.
		 * @customoverload
		 */
		auto vertex_indices() noexcept -> vertex_index_buffer_type& { return *_indices; }
		/** Returns the index of the vertices making up the shape. */
		auto vertex_indices() const noexcept -> const vertex_index_buffer_type& { return *_indices; }

		/** Returns the position of the shape.
		 * @customoverload
		 */
		auto position() noexcept -> position_buffer_type& { return _position; }
		/** Returns the position of the shape. */
		auto position() const noexcept -> const position_buffer_type& { return _position; }
		/** Returns the color of the shape.
		 * @customoverload
		 */
		auto color() noexcept -> position_buffer_type& { return _color; }
		/** Returns the color of the shape. */
		auto color() const noexcept -> const position_buffer_type& { return _color; }

		/** Returns the gpu that the shape is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> typename Implementation::gpu& { return vertices().gpu(); }
		/** Returns the gpu that the shape is on. */
		auto gpu() const noexcept -> const typename Implementation::gpu& { return vertices().gpu(); }

	public: // constructors
		/** Constructs an invalid [[simple_shape]].
		 * Using this shape is undefined behaviour.
		 * @overload
		 */
		simple_shape() = default;
		simple_shape(simple_shape&&) = default;
		auto operator=(simple_shape&&) -> simple_shape& = default;

		/**
		 * @param camera The camera this shape is displayed on.
		 * @param brush The brush used to draw this shape with.
		 * @param vertices The vertices making up this shape.
		 * Note that this data is stored outside of the [[simple_shape]].
		 * @param indices The indices making up this shape.
		 * Note that this data is stored outside of the [[simple_shape]].
		 * @param position The position of the shape.
		 * @param color The color of the shape.
		 */
		simple_shape(Implementation::camera& camera
			, simple_brush<Implementation>& brush
			, vertex_buffer_type& vertices, vertex_index_buffer_type& indices
			, float2 position = float2(), float3 color = float3())
			: _vertices(&vertices), _indices(&indices)
			, _position(camera.gpu(), 1)
			, _color(camera.gpu(), 1)
			, _drawable(camera, brush, vertices, indices, _position, _color)
		{
			if (position != float2()) _position.data()[0] = position;
			if (color != float3()) _color.data()[0] = color;
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_SIMPLE_SHAPE
