#ifndef COMPWOLF_TYPE_VALUE_PAIR_LIST
#define COMPWOLF_TYPE_VALUE_PAIR_LIST

#include <type_lists>
#include "type_value_pair.hpp"
#include <type_traits>

namespace compwolf
{
	namespace internal
	{
		template <typename PairList1, typename PairList2>
		struct merge_type_value_pairs_by_value
		{
			using types = typename PairList1::template and_type_list<PairList2>;
		};
		template <typename PairList1, typename PairList2>
			requires ((PairList1::empty && PairList2::empty) == false)
		struct merge_type_value_pairs_by_value<PairList1, PairList2>
		{
			using types = std::conditional_t<(PairList1::template at<0>::value < PairList2::template at<0>::value)
				, typename merge_type_value_pairs_by_value<typename PairList1::template back_subrange<PairList1::size - 1>, PairList2>
					::types::template and_types_at_front<typename PairList1::template at<0>>
				, typename merge_type_value_pairs_by_value<typename PairList2::template back_subrange<PairList2::size - 1>, PairList1>
					::types::template and_types_at_front<typename PairList2::template at<0>>
			>;
		};
	}

	/** Merges the given [[type_list]]s of [[type_value_pair]]s, where they are ordered by [[type_value_pair::value]].
	 * @warning It is undefined behaviour if the given lists are not already ordered.
	 */
	template <typename PairList1, typename PairList2>
	using merge_type_value_pairs_by_value = internal::merge_type_value_pairs_by_value<PairList1, PairList2>::types;
}

#endif // ! COMPWOLF_TYPE_VALUE_PAIR_LIST
