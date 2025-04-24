#ifndef COMPWOLF_GRAPHICS_GPU_DATA_INFO
#define COMPWOLF_GRAPHICS_GPU_DATA_INFO

#include <compwolf_type_traits>
#include <type_list>
#include <utility>

namespace compwolf
{
	/** A type trait class for types passed to the GPU.
	 * That is, to be able to pass your data type to the GPU, you must specialize this template with your type, so as to fulfill [[ShaderField]].
	 * You can use [[new_gpu_struct_info]] to easily create your specialization.
	 * @typeparam DataType The type to get info about, for the GPU.
	 */
	template <typename DataType>
	struct gpu_struct_info
	{
		static_assert(dependent_false<DataType>, "Tried getting information about a type that does not have its information defined");

		/** Primitives must list the primitive GPU types that make up the data.
		 * You can get this by using [[combine_primitive_gpu_types]] with the different types making up your data type.
		 */
		using primitives = type_list<>;
		/** For each [[gpu_struct_info::primitives]], its position in the type (in bytes). */
		static std::array<std::size_t, 0> primitive_offsets;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_DATA_INFO
