#include <gtest/gtest.h>
#include <enum_bitsets>

#include <concepts>

enum class simple_enum
{
	a, b, c, d,
	size
};
using simple_bitset = compwolf::enum_bitset<simple_enum>;

TEST(EnumBitset, inherits_bitset) {
	auto result = std::derived_from<
		simple_bitset,
		std::bitset<(std::size_t)simple_enum::size>
	>;
	EXPECT_TRUE(result);
}

TEST(EnumBitset, default_constructor) {
	const simple_bitset value;
	EXPECT_EQ(value.none(), true);
}
TEST(EnumBitset, enum_constructor) {
	const simple_bitset value(simple_enum::c);
	EXPECT_EQ(value[simple_enum::c], true);
	EXPECT_EQ(value.count(), 1);
}
TEST(EnumBitset, initializer_list_constructor) {
	simple_bitset value({ simple_enum::b, simple_enum::c });
	EXPECT_EQ(value[simple_enum::b], true);
	EXPECT_EQ(value[simple_enum::c], true);
	EXPECT_EQ(value.count(), 2);
}
TEST(EnumBitset, initializer_list_constructor_with_duplicates) {
	simple_bitset value({
		simple_enum::b,
		simple_enum::c,
		simple_enum::b,
		simple_enum::b,
		simple_enum::c
	});
	EXPECT_EQ(value[simple_enum::b], true);
	EXPECT_EQ(value[simple_enum::c], true);
	EXPECT_EQ(value.count(), 2);
}
TEST(EnumBitset, inherit_constructor) {
	simple_bitset value("0110");
	EXPECT_EQ(value[simple_enum::b], true);
	EXPECT_EQ(value[simple_enum::c], true);
	EXPECT_EQ(value.count(), 2);
}

TEST(EnumBitset, mutable_index) {
	simple_bitset value;
	value[simple_enum::c] = true;
	EXPECT_EQ(value[simple_enum::c], true);
	EXPECT_EQ(value.count(), 1);
}

TEST(EnumBitset, set) {
	simple_bitset value;
	value.set(simple_enum::c);
	EXPECT_EQ(value[simple_enum::c], true);
	EXPECT_EQ(value.count(), 1);
}
TEST(EnumBitset, set_false) {
	simple_bitset value("1111");
	value.set(simple_enum::c, false);
	EXPECT_EQ(value[simple_enum::c], false);
	EXPECT_EQ(value.count(), 3);
}
TEST(EnumBitset, set_chain) {
	simple_bitset value;
	value.set(simple_enum::c)
		.set(simple_enum::b);
	EXPECT_EQ(value[simple_enum::b], true);
	EXPECT_EQ(value[simple_enum::c], true);
	EXPECT_EQ(value.count(), 2);
}

TEST(EnumBitset, reset) {
	simple_bitset value("1111");
	value.reset(simple_enum::c);
	EXPECT_EQ(value[simple_enum::c], false);
	EXPECT_EQ(value.count(), 3);
}
TEST(EnumBitset, reset_chain) {
	simple_bitset value("1111");
	value.reset(simple_enum::c)
		.reset(simple_enum::b);
	EXPECT_EQ(value[simple_enum::b], false);
	EXPECT_EQ(value[simple_enum::c], false);
	EXPECT_EQ(value.count(), 2);
}

TEST(EnumBitset, flip) {
	simple_bitset value("0101");
	value.flip(simple_enum::b);
	value.flip(simple_enum::c);
	EXPECT_EQ(value[simple_enum::c], false);
	EXPECT_EQ(value[simple_enum::d], false);
	EXPECT_EQ(value.count(), 2);
}
TEST(EnumBitset, flip_chain) {
	simple_bitset value("0101");
	value.flip(simple_enum::b)
		.flip(simple_enum::c);
	EXPECT_EQ(value[simple_enum::c], false);
	EXPECT_EQ(value[simple_enum::d], false);
	EXPECT_EQ(value.count(), 2);
}