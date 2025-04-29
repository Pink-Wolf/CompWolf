#include <gtest/gtest.h>
#include <type_lists>
#include <concepts>
#include <tuple>

template <auto T>
struct test_type {};
template <auto T1, auto T2>
static bool operator==(test_type<T1> lhs, test_type<T2> rhs) { return T1 == T2; }
template <auto T1, auto T2>
static bool operator!=(test_type<T1> lhs, test_type<T2> rhs) { return T1 != T2; }

TEST(TypeList, size) {
	EXPECT_EQ((compwolf::type_list<int, float>::size), std::size_t(2));
	EXPECT_EQ((compwolf::type_list<int, float, int>::size), std::size_t(3));
	EXPECT_EQ((compwolf::type_list<int, int, int>::size), std::size_t(3));
	EXPECT_EQ((compwolf::type_list<>::size), std::size_t(0));
}
TEST(TypeList, empty) {
	EXPECT_EQ((compwolf::type_list<int, float>::empty), false);
	EXPECT_EQ((compwolf::type_list<int, float, int>::empty), false);
	EXPECT_EQ((compwolf::type_list<int, int, int>::empty), false);
	EXPECT_EQ((compwolf::type_list<>::empty), true);
}

TEST(TypeList, at) {
	EXPECT_EQ((compwolf::type_list<
		test_type<'a'>, test_type<'b'>
	>::at<0>()), test_type<'a'>());
	EXPECT_EQ((compwolf::type_list<
		test_type<'a'>, test_type<'b'>
	>::at<1>()), test_type<'b'>());
	EXPECT_EQ((compwolf::type_list<
		test_type<'a'>, test_type<'b'>, test_type<'a'>, test_type<'c'>
	>::at<2>()), test_type<'a'>());
}

TEST(TypeList, has) {
	EXPECT_EQ((compwolf::type_list<int, float>::has<int>), true);
	EXPECT_EQ((compwolf::type_list<int, float>::has<float>), true);
	EXPECT_EQ((compwolf::type_list<int, float>::has<bool>), false);

	EXPECT_EQ((compwolf::type_list<int, float, int>::has<int>), true);
	EXPECT_EQ((compwolf::type_list<int, float, int>::has<float>), true);
	EXPECT_EQ((compwolf::type_list<int, float, int>::has<bool>), false);

	EXPECT_EQ((compwolf::type_list<>::has<int>), false);
}

using and_types_t = compwolf::type_list<
	test_type<'a'>, test_type<'b'>
>::and_types<
	test_type<'c'>, test_type<'d'>
>;
TEST(TypeList, and_types) {
	EXPECT_EQ((and_types_t::at<0>()), test_type<'a'>());
	EXPECT_EQ((and_types_t::at<1>()), test_type<'b'>());
	EXPECT_EQ((and_types_t::at<2>()), test_type<'c'>());
	EXPECT_EQ((and_types_t::at<3>()), test_type<'d'>());
	EXPECT_EQ((and_types_t::size), std::size_t(4));
}
using and_types_empty_t = compwolf::type_list<
>::and_types<
	test_type<'a'>, test_type<'b'>
>;
TEST(TypeList, and_types_empty) {
	EXPECT_EQ((and_types_empty_t::at<0>()), test_type<'a'>());
	EXPECT_EQ((and_types_empty_t::at<1>()), test_type<'b'>());
	EXPECT_EQ((and_types_empty_t::size), std::size_t(2));
}

using and_types_at_front_t = compwolf::type_list<
	test_type<'c'>, test_type<'d'>
>::and_types_at_front<
	test_type<'a'>, test_type<'b'>
>;
TEST(TypeList, and_types_at_front) {
	EXPECT_EQ((and_types_at_front_t::at<0>()), test_type<'a'>());
	EXPECT_EQ((and_types_at_front_t::at<1>()), test_type<'b'>());
	EXPECT_EQ((and_types_at_front_t::at<2>()), test_type<'c'>());
	EXPECT_EQ((and_types_at_front_t::at<3>()), test_type<'d'>());
	EXPECT_EQ((and_types_at_front_t::size), std::size_t(4));
}
using and_types_at_front_empty_t = compwolf::type_list<
>::and_types_at_front<
	test_type<'a'>, test_type<'b'>
>;
TEST(TypeList, and_types_at_front_empty) {
	EXPECT_EQ((and_types_at_front_empty_t::at<0>()), test_type<'a'>());
	EXPECT_EQ((and_types_at_front_empty_t::at<1>()), test_type<'b'>());
	EXPECT_EQ((and_types_at_front_empty_t::size), std::size_t(2));
}

using and_type_list_t = compwolf::type_list<
	test_type<'a'>, test_type<'b'>
>::and_type_list<compwolf::type_list<
	test_type<'c'>, test_type<'d'>
>>;
TEST(TypeList, and_type_list) {
	EXPECT_EQ((and_type_list_t::at<0>()), test_type<'a'>());
	EXPECT_EQ((and_type_list_t::at<1>()), test_type<'b'>());
	EXPECT_EQ((and_type_list_t::at<2>()), test_type<'c'>());
	EXPECT_EQ((and_type_list_t::at<3>()), test_type<'d'>());
	EXPECT_EQ((and_type_list_t::size), std::size_t(4));
}
using and_type_list_empty_t = compwolf::type_list<
>::and_type_list<compwolf::type_list<
	test_type<'a'>, test_type<'b'>
	>>;
TEST(TypeList, and_type_list_empty) {
	EXPECT_EQ((and_type_list_empty_t::at<0>()), test_type<'a'>());
	EXPECT_EQ((and_type_list_empty_t::at<1>()), test_type<'b'>());
	EXPECT_EQ((and_type_list_empty_t::size), std::size_t(2));
}

template <typename T>
struct capitalize {};
template <auto T>
struct capitalize<test_type<T>>
{
	using type = test_type<T - 0x20>;
};
using transform_t = compwolf::type_list<
	test_type<'a'>, test_type<'b'>, test_type<'c'>, test_type<'d'>
>::transform<capitalize>;
TEST(TypeList, transform) {
	EXPECT_EQ((transform_t::at<0>()), test_type<'A'>());
	EXPECT_EQ((transform_t::at<1>()), test_type<'B'>());
	EXPECT_EQ((transform_t::at<2>()), test_type<'C'>());
	EXPECT_EQ((transform_t::at<3>()), test_type<'D'>());
	EXPECT_EQ((transform_t::size), std::size_t(4));

	EXPECT_EQ((compwolf::type_list<>::transform<capitalize>::size), std::size_t(0));
}

TEST(TypeList, to_other_container) {
	EXPECT_EQ(
		(compwolf::type_list<int, float>::to_other_container<std::tuple>()),
		(std::tuple<int, float>())
	);
	EXPECT_EQ(
		(compwolf::type_list<int, float, int, bool>::to_other_container<std::tuple>()),
		(std::tuple<int, float, int, bool>())
	);

	EXPECT_EQ(
		(compwolf::type_list<>::to_other_container<std::tuple>()),
		(std::tuple<>())
	);
}

using to_type_list_t = compwolf::to_type_list<std::tuple<
	test_type<'a'>, test_type<'b'>
>>;
TEST(TypeList, to_type_list) {
	EXPECT_EQ((to_type_list_t::at<0>()), test_type<'a'>());
	EXPECT_EQ((to_type_list_t::at<1>()), test_type<'b'>());
	EXPECT_EQ((to_type_list_t::size), std::size_t(2));
}
using to_type_list_empty_t = compwolf::to_type_list<std::tuple<>>;
TEST(TypeList, to_type_list_empty) {
	EXPECT_EQ((to_type_list_empty_t::size), std::size_t(0));
}

using combine_type_lists_t = compwolf::combine_type_lists<
	compwolf::type_list<
		test_type<'a'>, test_type<'b'>
	>, compwolf::type_list<
		test_type<'c'>, test_type<'d'>
>>;
TEST(TypeList, combine_type_lists) {
	EXPECT_EQ((combine_type_lists_t::at<0>()), test_type<'a'>());
	EXPECT_EQ((combine_type_lists_t::at<1>()), test_type<'b'>());
	EXPECT_EQ((combine_type_lists_t::at<2>()), test_type<'c'>());
	EXPECT_EQ((combine_type_lists_t::at<3>()), test_type<'d'>());
	EXPECT_EQ((combine_type_lists_t::size), std::size_t(4));
}

TEST(TypeList, is_type_list_v) {
	EXPECT_EQ((compwolf::is_type_list_v<compwolf::type_list<int, float>>), true);
	EXPECT_EQ((compwolf::is_type_list_v<compwolf::type_list<>>), true);
	EXPECT_EQ((compwolf::is_type_list_v<std::tuple<int, float>>), false);
	EXPECT_EQ((compwolf::is_type_list_v<std::tuple<>>), false);
}
TEST(TypeList, typelist) {
	EXPECT_EQ((compwolf::typelist<compwolf::type_list<int, float>>), true);
	EXPECT_EQ((compwolf::typelist<compwolf::type_list<>>), true);
	EXPECT_EQ((compwolf::typelist<std::tuple<int, float>>), false);
	EXPECT_EQ((compwolf::typelist<std::tuple<>>), false);
}
