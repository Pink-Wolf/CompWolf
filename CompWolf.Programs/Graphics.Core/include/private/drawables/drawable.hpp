#ifndef COMPWOLF_GRAPHICS_DRAWABLE
#define COMPWOLF_GRAPHICS_DRAWABLE

#include <graphics_environments>
#include <shaders>
#include <gpu_buffers>
#include <gpu_structs>
#include "brush.hpp"
#include <tuple>
#include <utility>
#include <stdexcept>
#include <concepts>

namespace compwolf
{
	namespace internal
	{
		/** @hidden */
		template <template <typename> typename BufferType>
		struct field_buffer
		{
			template <typename T>
			struct transformer
			{
				using type = BufferType<T>;
			};
			template <typename T>
			struct ptr_transformer
			{
				using type = BufferType<T>*;
			};
		};
	}

	/** Represents some object that is drawn on a window.
	 *
	 * @typeparam BrushType The type of brush that can be used to draw this.
	 * @typeparam CameraType The type of camera this drawable can be on.
	 * @typeparam BufferType The type of buffer used to keep the drawable's data.
	 */
	template <typename BrushType, typename CameraType, template <gpu_buffer_usage, typename> typename BufferType>
	class drawable
	{
	public:
		/** The type of [[graphics_environment]] that this drawable works with. */
		using environment_type = typename BrushType::environment_type;
		/** The type of camera this drawable can be on. */
		using camera_type = CameraType;
		/** The type of brush that can be used to draw this. */
		using brush_type = BrushType;
		/** The type of buffer used to keep the drawable's data. */
		template <gpu_buffer_usage UsageType, typename ElementType>
		using buffer_type = BufferType<UsageType, ElementType>;

		/** The type of buffer used to keep the vertices of the drawable.
		 * @see drawable::vertex_index_buffer_type
		 */
		using vertex_buffer_type = buffer_type<gpu_buffer_usage::input, typename BrushType::input_type>;
		/** The type of buffer used to keep the index of the vertices that makes up the triangles of the drawable.
		 * @see drawable::vertex_buffer_type
		 */
		using vertex_index_buffer_type = buffer_type<gpu_buffer_usage::input_index, shader_int>;

		/** The type of buffer used to keep the uniform data of the drawable. */
		template <typename T>
		using field_buffer_type = buffer_type<gpu_buffer_usage::field, T>;
		/** For each field of the drawable, the type of buffer used to keep that data.
		 * This is in a [[type_list]].
		 */
		using field_buffer_types = typename brush_type::field_types
			::template transform<
				internal::field_buffer<field_buffer_type>::transformer
			>
		;
		/** For each field of the drawable, the type of pointer to the buffer used to keep that data.
		 * This is in a std::tuple.
		 */
		using field_buffer_ptr_tuple = typename brush_type::field_types
			::template transform<
				internal::field_buffer<field_buffer_type>::ptr_transformer
			>
			::template to_other_container<std::tuple>
		;

	private:
		camera_type* _camera{};
		brush_type* _brush{};
		vertex_buffer_type* _vertex_buffer{};
		vertex_index_buffer_type* _vertex_index_buffer{};
		field_buffer_ptr_tuple _field_buffers{};

	public: // accessors
		/** Returns the gpu that the drawable is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> typename environment_type::gpu_type& { return _vertex_buffer->gpu(); }
		/** Returns the gpu that the drawable is on. */
		auto gpu() const noexcept -> const typename environment_type::gpu_type& { return _vertex_buffer->gpu(); }

		/** Returns the camera that the drawable is on.
		 * @customoverload
		 */
		auto camera() noexcept -> camera_type& { return *_camera; }
		/** Returns the camera that the drawable is on. */
		auto camera() const noexcept -> const camera_type& { return *_camera; }

		/** Returns the brush used to draw the drawable.
		 * @customoverload
		 */
		auto brush() noexcept -> brush_type& { return *_brush; }
		/** Returns the brush used to draw the drawable. */
		auto brush() const noexcept -> const brush_type& { return *_brush; }

		/** Returns the drawable's vertices.
		 * @customoverload
		 */
		auto vertex_buffer() noexcept -> vertex_buffer_type& { return *_vertex_buffer; }
		/** Returns the drawable's vertices. */
		auto vertex_buffer() const noexcept -> const vertex_buffer_type& { return *_vertex_buffer; }

		/** Returns the drawable's vertex indices. That is, the index of the vertices making up the drawable's triangles.
		 * @customoverload
		 */
		auto vertex_index_buffer() noexcept -> vertex_index_buffer_type& { return *_vertex_index_buffer; }
		/** Returns the drawable's vertex indices. That is, the index of the vertices making up the drawable's triangles. */
		auto vertex_index_buffer() const noexcept -> const vertex_index_buffer_type& { return *_vertex_index_buffer; }

		/** Returns the drawable's uniform data, in a tuple. */
		auto field_buffers() const noexcept -> const field_buffer_ptr_tuple& { return *_field_buffers; }

	private:
		template <std::size_t Step>
		auto field_buffer_at_internal(std::size_t index) const noexcept -> const void*
		{
			if constexpr (Step == std::tuple_size_v<field_buffer_ptr_tuple>)
				throw std::out_of_range("Tried getting the field buffer with an index equal to or greater than the number of fields");
			else
			{
				if (index == Step)
					return static_cast<const void*>(std::get<Step>(_field_buffers));
				else
					return field_buffer_at_internal<Step + 1>(index);
			}
		}
	public:
		/** Returns the drawable's uniform data. */
		auto field_buffer_at(std::size_t index) const noexcept -> const void*
		{
			return field_buffer_at_internal<0>(index);
		}
		/** Returns the drawable's uniform data.
		 * @customoverload
		 */
		auto field_buffer_at(std::size_t index) noexcept -> void*
		{
			return const_cast<void*>(field_buffer_at_internal<0>(index));
		}
		/** Returns the drawable's uniform data.
		 * @customoverload
		 */
		template <std::size_t Index>
		auto field_buffer_at() const noexcept -> const std::tuple_element_t<Index, field_buffer_ptr_tuple>&
		{
			return *std::get<Index>(_field_buffers);
		}
		/** Returns the drawable's uniform data.
		 * @customoverload
		 */
		template <std::size_t Index>
		auto field_buffer_at() noexcept -> std::tuple_element_t<Index, field_buffer_ptr_tuple>&
		{
			return *std::get<Index>(_field_buffers);
		}

	public: // constructors
		/** Constructs an invalid [[drawable]].
		 * Using this drawable is undefined behaviour.
		 * @overload
		 */
		drawable() = default;
		drawable(drawable&&) = default;
		auto operator=(drawable&&) -> drawable& = default;

		/** Creates a drawable using the given brush and data.
		 * @customoverload
		 */
		drawable(camera_type& camera, brush_type& brush
			, vertex_buffer_type& vertex_buffer, vertex_index_buffer_type& vertex_index_buffer
			, field_buffer_ptr_tuple field_ptrs)
			: _camera(&camera), _brush(&brush)
			, _vertex_buffer(&vertex_buffer), _vertex_index_buffer(&vertex_index_buffer)
			, _field_buffers(std::move(field_ptrs))
		{
		}
		/** Creates a drawable using the given brush and data. */
		template <typename... FieldBufferTypes>
			requires (std::same_as<type_list<FieldBufferTypes...>, field_buffer_types>)
		drawable(camera_type& camera, brush_type& brush
			, vertex_buffer_type& vertex_buffer, vertex_index_buffer_type& vertex_index_buffer
			, FieldBufferTypes&... field_buffers)
			: drawable(camera, brush, vertex_buffer, vertex_index_buffer, std::make_tuple(&field_buffers...))
		{ }
	};
}

#endif // ! COMPWOLF_GRAPHICS_DRAWABLE
