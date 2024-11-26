#ifndef COMPWOLF_DEREFERENCEABLE_TO
#define COMPWOLF_DEREFERENCEABLE_TO

#include <type_traits>
#include <concepts>
#include <utility>

namespace compwolf
{
	/** Whether dereferencing an object of type Pointer returns an object of type Result. */
	template <typename Pointer, typename Result>
	concept dereferenceable_to = requires (Pointer p)
	{
		{ *p } -> std::convertible_to<Result>;
	};
	/** Whether dereferencing an object of type Pointer returns an object of type Result. */
	template <typename Pointer, typename Result>
	struct is_dereferenceable_to : std::false_type {};
	template <typename Pointer, typename Result>
		requires dereferenceable_to<Pointer, Result>
	struct is_dereferenceable_to<Pointer, Result> : std::true_type {};
	/** Whether dereferencing an object of type Pointer returns an object of type Result. */
	template <typename Pointer, typename Result>
	constexpr bool is_dereferenceable_to_v = is_dereferenceable_to<Pointer, Result>::value;


	/** Whether dereferencing an object of type Pointer, and converting the result to Result, is noexcept. */
	template <typename Pointer, typename Result>
	concept nothrow_dereferenceable_to =
		std::is_nothrow_convertible_v<decltype(*std::declval<Pointer>()), Result>
		&& requires (Pointer p)
	{
		noexcept(noexcept(*p));
	};
	/** Whether dereferencing an object of type Pointer, and converting the result to Result, is noexcept. */
	template <typename Pointer, typename Result>
	struct is_nothrow_dereferenceable_to : std::false_type {};
	template <typename Pointer, typename Result>
		requires nothrow_dereferenceable_to<Pointer, Result>
	struct is_nothrow_dereferenceable_to<Pointer, Result> : std::true_type {};
	/** Whether dereferencing an object of type Pointer, and converting the result to Result, is noexcept. */
	template <typename Pointer, typename Result>
	constexpr bool is_nothrow_dereferenceable_to_v = is_nothrow_dereferenceable_to<Pointer, Result>::value;
}

#endif // ! COMPWOLF_DEREFERENCEABLE_TO
