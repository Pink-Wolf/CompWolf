#ifndef COMPWOLF_IS_TYPE_LIST
#define COMPWOLF_IS_TYPE_LIST

#include <concepts>

namespace compwolf
{
	namespace internal
	{
		struct type_list_base;
	}

	/** Whether the given type is a [[type_list]]. */
	template <typename T>
	concept typelist = std::derived_from<T, internal::type_list_base>;
	/** Whether the given type is a [[type_list]]. */
	template <typename T>
	struct is_type_list : std::false_type {};
	template <typelist T>
	struct is_type_list<T> : std::true_type {};
	/** Whether the given type is a [[type_list]]. */
	template <typename T>
	constexpr bool is_type_list_v = is_type_list<T>::value;
}

#endif // ! COMPWOLF_IS_TYPE_LIST
