#ifndef COMPWOLF_TYPE_LIST
#define COMPWOLF_TYPE_LIST

#include <compwolf_type_traits>
#include "utility"
#include "is_type_list.hpp"

namespace compwolf
{
	template <typename... Ts>
	struct type_list;

	namespace internal
	{
		/** @hidden */
		template <std::size_t Index, typename FrontType, typename... TailTypes>
		struct get_type_at_index_internal
		{
			using type = get_type_at_index_internal<Index - 1, TailTypes...>::type;
		};
		template <typename FrontType, typename... TailTypes>
		struct get_type_at_index_internal<0, FrontType, TailTypes...>
		{
			using type = FrontType;
		};
		/** @hidden */
		template <std::size_t Index, typename... Types>
		struct get_type_at_index
		{
			static_assert(Index < sizeof...(Types), "out of range! type_list::at was given an index greater than or equal to the amount of elements");
			using type = get_type_at_index_internal<Index, Types...>::type;
		};

		struct type_list_base {};

		template <typename TemplateInstance>
		struct to_type_list
		{
			static_assert(dependent_false<TemplateInstance>, "to_type_list was not given an instance of a template");
		};
		template <template <typename...> typename Template, typename... Ts>
		struct to_type_list<Template<Ts...>>
		{
			using type = type_list<Ts...>;
		};
		template <template <typename> typename Template, typename T>
		struct to_type_list<Template<T>>
		{
			using type = type_list<T>;
		};

		/** @hidden */
		template <typelist... TypeLists>
		struct combine_type_lists
		{
			using type = type_list<>;
		};
		template <typelist Head, typename... Tail>
		struct combine_type_lists<Head, Tail...>
		{
			using type = typename Head::template and_type_list<
				typename combine_type_lists<Tail...>::type
			>;
		};
	}

	/** A random-access container of types.
	 * The advantage of this over [[std::tuple]] is that this is explicitly a type list, and therefore contains relevant functionality like [[type_list::at]].
	 */
	template <typename... Ts>
	struct type_list : internal::type_list_base
	{
		/** The amount of types contained by the [[type_list]]. */
		static constexpr std::size_t size = sizeof...(Ts);
		/** Whether the [[type_list]] has any members. */
		static constexpr bool empty = false;

		/** The type at the given index. */
		template <std::size_t Index>
		using at = internal::get_type_at_index<Index, Ts...>::type;

		/** Whether the type list has the given type at any point. */
		template <typename T>
		static constexpr bool has = (std::is_same_v<Ts, T> || ...);

		/** A copy of the [[type_list]] with the given types appended to it. */
		template <typename... TOthers>
		using and_types = type_list<Ts..., TOthers...>;
		/** A copy of the [[type_list]] with the given types appended to it in, in front of the original types. */
		template <typename... TOthers>
		using and_types_at_front = type_list<TOthers..., Ts...>;
		/** A copy of the [[type_list]] with the given [[type_list]]'s types appended to it. */
		template <typelist TypeList>
		using and_type_list = TypeList::template and_types_at_front<Ts...>;

		/** A copy of the [[type_list]], but where each type T is replaced by Transformer<T>::type. */
		template <template <typename> typename Transformer>
		using transform = type_list<typename Transformer<Ts>::type...>;

		/** The given template, instantiated with the types in the [[type_list]]. */
		template <template <typename...> typename TypeList>
		using to_other_container = TypeList<Ts...>;
	};

	/** A random-access container of types.
	 * The advantage of this over [[std::tuple]] is that this is explicitly a type list, and therefore contains relevant functionality like [[type_list::at]].
	 */
	template <>
	struct type_list<> : internal::type_list_base
	{
		/** The amount of types contained by the [[type_list]]. */
		static constexpr std::size_t size = 0;
		/** Whether the [[type_list]] has any members. */
		static constexpr bool empty = true;

		/** Whether the type list has the given type at any point. */
		template <typename T>
		static constexpr bool has = false;

		/** A copy of the [[type_list]] with the given types appended to it. */
		template <typename... TOthers>
		using and_types = type_list<TOthers...>;
		/** A copy of the [[type_list]] with the given types appended to it in, in front of the original types. */
		template <typename... TOthers>
		using and_types_at_front = type_list<TOthers...>;

		/** A copy of the [[type_list]] with the given [[type_list]]'s types appended to it. */
		template <typelist TypeList>
		using and_type_list = TypeList;

		/** A copy of the [[type_list]], but where each type T is replaced by Transformer<T>::type. */
		template <template <typename> typename Transformer>
		using transform = type_list<>;

		/** The given template, instantiated with the types in the [[type_list]]. */
		template <template <typename...> typename TypeList>
		using to_other_container = TypeList<>;
	};

	/** A [[type_list]] containing the types used in the given template instance.
	 * For example:
	 * * std::vector<int> would give compwolf::type_list<int>.
	 * * std::tuple<int, float> would give compwolf::type_list<int, float>.
	 */
	template <typename TemplateInstance>
	using to_type_list = internal::to_type_list<TemplateInstance>::type;

	/** A single [[type_list]] with the elements of the given [[type_list]]s. */
	template <typelist... TypeLists>
	using combine_type_lists = internal::combine_type_lists<TypeLists...>::type;
}

#endif // ! COMPWOLF_TYPE_LIST
