#ifndef COMPWOLF_VERSION_UNIQUE_DELETER_PTR
#define COMPWOLF_VERSION_UNIQUE_DELETER_PTR

#include <memory>
#include <utility>
#include <functional>
#include <type_traits>
#include <cstddef>

namespace compwolf
{
	namespace internal
	{
		/** @hidden */
		template <typename Pointer>
		class unique_deleter_ptr_deleter : public std::function<void(Pointer)>
		{
		public:
			using pointer = Pointer;
			using std::function<void(Pointer)>::function;
		};
	}

	template<typename T>
	class unique_deleter_ptr
		: public std::unique_ptr<T, internal::unique_deleter_ptr_deleter<T*>>
	{
	private:
		using super = std::unique_ptr<T, internal::unique_deleter_ptr_deleter<T*>>;
	public: // constructors
		using typename super::unique_ptr;

		constexpr unique_deleter_ptr() noexcept(std::is_nothrow_default_constructible_v<T*>)
			: super(nullptr, nullptr)
		{}

		constexpr unique_deleter_ptr(T* p) noexcept
			: super(p, nullptr)
		{}

		template<typename DeleterArg>
		constexpr unique_deleter_ptr(T* p, DeleterArg&& f)
			noexcept(std::is_nothrow_constructible_v<typename super::deleter_type, DeleterArg&&>)
			: super(p, typename super::deleter_type(std::forward<DeleterArg>(f)))
		{}

		constexpr unique_deleter_ptr(const unique_deleter_ptr&) noexcept = default;
		constexpr unique_deleter_ptr& operator=(const unique_deleter_ptr&) noexcept = default;
		constexpr unique_deleter_ptr(unique_deleter_ptr&&) noexcept = default;
		constexpr unique_deleter_ptr& operator=(unique_deleter_ptr&&) noexcept = default;
	};

	template<typename T>
	class unique_deleter_ptr<T[]>
		: public std::unique_ptr<T[], internal::unique_deleter_ptr_deleter<T*>>
	{
	private:
		using super = std::unique_ptr<T[], internal::unique_deleter_ptr_deleter<T*>>;
	public: // constructors
		using typename super::unique_ptr;

		constexpr unique_deleter_ptr() noexcept(std::is_nothrow_default_constructible_v<T*>)
			: super(nullptr, nullptr)
		{}

		constexpr unique_deleter_ptr(T* p) noexcept
			: super(p, nullptr)
		{}

		template<typename DeleterArg>
		constexpr unique_deleter_ptr(T* p, DeleterArg&& f)
			noexcept(std::is_nothrow_constructible_v<typename super::deleter_type, DeleterArg&&>)
			: super(p, typename super::deleter_type(std::forward<DeleterArg>(f)))
		{}

		constexpr unique_deleter_ptr(const unique_deleter_ptr&) noexcept = default;
		constexpr unique_deleter_ptr& operator=(const unique_deleter_ptr&) noexcept = default;
		constexpr unique_deleter_ptr(unique_deleter_ptr&&) noexcept = default;
		constexpr unique_deleter_ptr& operator=(unique_deleter_ptr&&) noexcept = default;
	};
}

#endif // ! COMPWOLF_VERSION_UNIQUE_DELETER_PTR
