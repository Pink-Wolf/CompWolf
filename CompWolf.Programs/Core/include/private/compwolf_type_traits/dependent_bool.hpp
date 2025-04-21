#ifndef COMPWOLF_DEPENDENT_BOOL
#define COMPWOLF_DEPENDENT_BOOL

namespace compwolf
{
	/** False, but only evaluated at the second phase of name lookup. */
	template <typename... Ts>
	constexpr bool dependent_false = sizeof...(Ts) < 0;
	/** True, but only evaluated at the second phase of name lookup. */
	template <typename... Ts>
	constexpr bool dependent_true = sizeof...(Ts) >= 0;
}

#endif // ! COMPWOLF_DEPENDENT_BOOL
