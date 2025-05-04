#ifndef COMPWOLF_GRAPHICS_SIMPLE_SQUARE
#define COMPWOLF_GRAPHICS_SIMPLE_SQUARE

#include "simple_shape.hpp"
#include <utility>
#include <map>

namespace compwolf
{
	/** A simple square.
	 * Its fields are:
	 * * The square's position, as a [[float2]].
	 * * The square's RGB color, as a [[float3]].
	 * @typeparam Implementation The implementation of [[CompWolf.Graphics]] to use.
	 */
	template <ImplementationType Implementation = default_implementation>
	class simple_square : public simple_shape<Implementation>
	{
		using super = simple_shape<Implementation>;

		struct gpu_data
		{
			std::size_t instance_count;
			super::vertex_buffer_type vertices;
			super::vertex_index_buffer_type indices;
		};

		static inline std::map<typename Implementation::gpu*, gpu_data> _gpu_data{};
		static auto get_gpu_data(Implementation::gpu& gpu) -> gpu_data&
		{
			auto key = &gpu;
			// get existing
			{
				auto i = _gpu_data.find(key);
				if (i != _gpu_data.end()) return i->second;
			}

			// construct if not existing
			gpu_data data
			{
				.instance_count = 0,
				.vertices = super::vertex_buffer_type(gpu, 4),
				.indices = super::vertex_index_buffer_type(gpu, 6),
			};
			{
				auto v = data.vertices.data();
				v[0] = { -1.f, -1.f };
				v[1] = { -1.f, +1.f };
				v[2] = { +1.f, -1.f };
				v[3] = { +1.f, +1.f };
			}
			{
				auto i = data.indices.data();
				i[0] = 0;
				i[1] = 2;
				i[2] = 1;
				i[3] = 1;
				i[4] = 2;
				i[5] = 3;
			}

			return _gpu_data.emplace(key, std::move(data))
				.first->second;
		}

	public: // constructors
		/** Constructs an invalid [[simple_square]].
		 * Using this shape is undefined behaviour.
		 * @overload
		 */
		simple_square() = default;
		simple_square(simple_square&&) = default;
		auto operator=(simple_square&&) -> simple_square& = default;

		/**
		 * @param camera The camera this shape is displayed on.
		 * @param brush The brush used to draw this shape with.
		 * @param vertices The vertices making up this shape.
		 * Note that this data is stored outside of the [[simple_square]].
		 * @param indices The indices making up this shape.
		 * Note that this data is stored outside of the [[simple_square]].
		 * @param transform The position and scale of the shape.
		 * @param color The color of the shape.
		 */
		simple_square(Implementation::camera& camera
			, simple_brush<Implementation>& brush
			, simple_transform_data transform = simple_transform_data(), float3 color = float3())
			: super(camera, brush, get_gpu_data(camera.gpu()).vertices, get_gpu_data(camera.gpu()).indices, transform, color)
		{
			++get_gpu_data(super::gpu()).instance_count;
		}

		~simple_square() noexcept
		{
			auto i = _gpu_data.find(&super::gpu());
			auto& gpu_data = i->second;

			if (--gpu_data.instance_count <= 0)
			{
				_gpu_data.erase(i);
			}
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_SIMPLE_SQUARE
