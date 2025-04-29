#ifndef COMPWOLF_TYPE_VALUE_PAIR
#define COMPWOLF_TYPE_VALUE_PAIR

namespace compwolf
{
	/** A compile-time pairing of a type and a value. */
	template <typename Type, auto Value>
	struct type_value_pair
	{
		using type = Type;
		static constexpr auto value = Value;
	};
}

#endif // ! COMPWOLF_TYPE_VALUE_PAIR
