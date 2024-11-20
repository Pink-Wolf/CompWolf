#ifndef COMPWOLF_FREEABLE
#define COMPWOLF_FREEABLE

#include <type_traits>
#include <memory>
#include <utility>

namespace compwolf
{
	/** An object whose data can be manually freed. */
	template <typename T>
	concept Freeable = requires (T t)
	{
		/** Can get whether an object is freed. */
		{ t.empty() } -> std::convertible_to<bool>;
		/** An object's data can be freed. */
		{ t.free() };
	};
	/** An object whose data can be manually freed. */
	template <typename T>
	struct is_freeable : std::false_type {};
	template <Freeable T>
	struct is_freeable<T> : std::true_type {};
	/** An object whose data can be manually freed. */
	template <typename T>
	static constexpr bool is_freeable_v = Freeable<T>;

	/** Interface denoting that this object's data can be manually freed. */
	struct freeable
	{
		/** Whether the object is freed. */
		virtual auto empty() const noexcept -> bool = 0;
		/** Frees the object's data. */
		virtual void free() noexcept = 0;

	};

	/** A freeable which calls free when assigned.
	 * A type inheriting from this should generally invoke free when destroyed.
	 */
	struct basic_freeable : public freeable
	{
		basic_freeable() = default;
		basic_freeable(const basic_freeable&) = default;
		auto operator=(const basic_freeable&) noexcept -> basic_freeable&
		{
			free();
			return *this;
		}
		basic_freeable(basic_freeable&&) = default;
		auto operator=(basic_freeable&&) noexcept -> basic_freeable&
		{
			free();
			return *this;
		}
		~basic_freeable() = default;
	};
}

#endif // ! COMPWOLF_FREEABLE
