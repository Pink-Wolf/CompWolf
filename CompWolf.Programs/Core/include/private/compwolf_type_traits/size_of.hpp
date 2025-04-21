#ifndef COMPWOLF_SIZE_OF
#define COMPWOLF_SIZE_OF

#include <utility>

namespace compwolf
{
	/** The amount of template arguments given to this. */
	template <auto... Ts>
	constexpr std::size_t size_of = sizeof...(Ts);
}

#endif // ! COMPWOLF_SIZE_OF
