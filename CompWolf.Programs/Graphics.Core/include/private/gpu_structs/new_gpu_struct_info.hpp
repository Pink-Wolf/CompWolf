#ifndef COMPWOLF_GRAPHICS_NEW_GPU_STRUCT_INFO
#define COMPWOLF_GRAPHICS_NEW_GPU_STRUCT_INFO

#include "gpu_struct_info.hpp"
#include <compwolf_type_traits>

namespace compwolf
{
	namespace internal
	{
		/** @hidden */
		template <auto Delta>
		struct new_gpu_struct_info_add
		{
			template <typename Pair>
			struct to_pair
			{
				static_assert(dependent_false<Pair>, "The given type was not a type value pair");
			};
			template <typename Type, auto Value>
			struct to_pair<type_value_pair<Type, Value>>
			{
				using type = type_value_pair<Type, Value + Delta>;
			};
		};
	}

	/** Implements everything a specialization of [[gpu_struct_info]] needs, which means specializations can just inherit from this instead of manually implementing the fields.
	 * @typeparam FieldAndOffsets Must be some [[type_value_pair]]s, specifying the target type's various data members.
	 * The pairs should specifically specify their type and offset (offset can be calculated with offsetof).
	 * All specified types must be passable to the gpu themselves.
	 * @see gpu_struct_info
	 */
	template <typename... FieldAndOffsets>
	struct new_gpu_struct_info
	{
		using primitives = combine_type_lists<
			typename gpu_struct_info<typename FieldAndOffsets::type>::primitives
			::template transform<internal::template new_gpu_struct_info_add<FieldAndOffsets::value>::template to_pair>
			...
		>;
	};
}

#endif // ! COMPWOLF_GRAPHICS_NEW_GPU_STRUCT_INFO
