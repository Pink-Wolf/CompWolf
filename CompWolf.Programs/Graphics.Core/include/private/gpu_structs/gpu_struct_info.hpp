#ifndef COMPWOLF_GRAPHICS_GPU_DATA_INFO
#define COMPWOLF_GRAPHICS_GPU_DATA_INFO

#include <compwolf_type_traits>
#include <type_lists>
#include <type_value_pairs>
#include <utility>

namespace compwolf
{
	/** A type trait class for types passed to the GPU.
	 * That is, to be able to pass your data type to the GPU, you must specialize this template with your type.
	 * You can use [[new_gpu_struct_info]] to easily create your specialization.
	 * @typeparam DataType The type to get info about, for the GPU.
	 */
	template <typename DataType>
	struct gpu_struct_info
	{
		static_assert(dependent_false<DataType>, "Tried getting information about a type that does not have its information defined");

		/** Primitives must list the primitive GPU types that make up the data.
		 * The list must actually be made out of [[type_value_pair]]s, paring the type with its position in the containing type.
		 */
		using primitives = type_list<type_value_pair<void, 0>>;
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_DATA_INFO
