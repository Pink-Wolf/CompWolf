#ifndef COMPWOLF_GRAPHICS_VULKAN_DRAWABLE
#define COMPWOLF_GRAPHICS_VULKAN_DRAWABLE

#include <vulkan_graphics_environments>
#include <drawables>
#include "vulkan_brush.hpp"
#include <vulkan_windows>
#include <vulkan_gpu_buffers>
#include <array>
#include <span>

namespace compwolf::vulkan
{
	template <typename BrushType>
	class vulkan_drawable;

	namespace internal
	{
		/** @hidden */
		void drawable_draw_code(const vulkan_draw_code_parameters&
			, vulkan_handle::pipeline
			, shader_int vertex_index_count
			, vulkan_handle::buffer vertex_buffer
			, vulkan_handle::buffer vertex_index_buffer
			, const vulkan_handle::buffer* field_buffers_data
			, const std::vector<shader_int>& field_indices
			, const std::vector<vulkan_handle::descriptor_set>&
		);
	}

	/** A Vulkan-implementation of [[drawable]].
	 * @typeparam BrushType The type of brush that can be used to draw this.
	 * @see drawable
	 * @see vulkan_graphics_environment
	 */
	template <typename BrushType>
	class vulkan_drawable
		: public drawable<BrushType, vulkan_camera, vulkan_gpu_buffer>
	{
		using super = drawable<BrushType, vulkan_window, vulkan_gpu_buffer>;

		vulkan_camera::draw_code_key _draw_key;

		std::array<vulkan_handle::memory, super::field_buffer_types::size> _field_memories;
		std::array<vulkan_handle::buffer, super::field_buffer_types::size> _field_buffer;

		template <std::size_t Step>
		constexpr void setup_field_data()
		{
			if constexpr (Step < super::field_buffer_types::size)
			{
				auto& field = std::get<Step>(super::field_buffers());
				_field_memories[Step] = field.vulkan_memory();
				_field_buffer[Step] = field.vulkan_buffer();
				setup_field_data<Step + 1>();
			}
		}

	public: // vulkan-specific
		/** Returns the [[vulkan_handle::memory]]s making up the drawable's fields. */
		auto vulkan_field_memories() const noexcept
			-> std::span<const vulkan_handle::memory, super::field_buffer_types::size>
		{
			return std::span<const vulkan_handle::memory, super::field_buffer_types::size>
				(_field_memories.data(), _field_memories.size());
		}
		/** Returns the [[vulkan_handle::buffer]]s making up the drawable's fields. */
		auto vulkan_field_buffers() const noexcept
			-> std::span<const vulkan_handle::buffer, super::field_buffer_types::size>
		{
			return std::span<const vulkan_handle::buffer, super::field_buffer_types::size>
				(_field_buffer.data(), _field_buffer.size());
		}

	public: //
		/** The gpu-instructions used to draw this. */
		void draw_program_code(const vulkan_draw_code_parameters& args)
		{
			drawable_draw_code(args
				, super::brush().vulkan_pipeline()
				, super::vertex_buffer().vulkan_buffer()
				, super::vertex_index_buffer().vulkan_buffer()
				, vulkan_field_buffers().data()
				, super::brush().field_positions()
				, super::brush().vulkan_descriptor_set(super::camera().window())
			);
		}

	public: // constructors
		/** Constructs an invalid [[vulkan_drawable]].
		 * Using this drawable is undefined behaviour.
		 * @overload
		 */
		vulkan_drawable() = default;
		vulkan_drawable(vulkan_drawable&&) = default;
		auto operator=(vulkan_drawable&&) -> vulkan_drawable& = default;

		/** Creates a drawable using the given brush and data.
		 * @customoverload
		 */
		vulkan_drawable(super::camera_type& camera, super::brush_type& brush
			, super::vertex_buffer_type& vertex_data, super::vertex_index_buffer_type& vertex_index_data
			, super::field_buffer_ptr_tuple field_ptrs)
			: super(camera, brush, vertex_data, vertex_index_data, field_ptrs)
		{
			_draw_key = camera.add_draw_code(
				[this](const vulkan_draw_code_parameters& args) { draw_program_code(args); }
			);

			setup_field_data<0>();
		}
		/** Creates a drawable using the given brush and data. */
		template <typename... FieldBufferTypes>
			requires (std::same_as<type_list<FieldBufferTypes...>, typename super::field_buffer_type>)
		vulkan_drawable(super::camera_type& camera, super::brush_type& brush
			, super::vertex_buffer_type& vertex_data, super::vertex_index_buffer_type& vertex_index_data
			, FieldBufferTypes&... fields)
			: vulkan_drawable(camera, brush, vertex_data, vertex_index_data, std::make_tuple(&fields...))
		{ }
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_DRAWABLE
