#include <gtest/gtest.h>
#include <events>

struct test_type
{
	mutable int value;

	test_type(int i) : value(i) {}
};

using test_listenable = compwolf::listenable<test_type>;

TEST(Listenable, single_observer_pair) {
	test_listenable x(-1);
	int old_before, new_before, old_after, new_after;

	x.value_updating().subscribe([&old_before, &new_before](test_listenable::update_parameters p)
		{
			old_before = (p.old_value.value *= 2);
			new_before = (p.new_value.value *= 3);
		});
	x.value_updated().subscribe([&old_after, &new_after](test_listenable::update_parameters p)
		{
			old_after = (p.old_value.value *= 5);
			new_after = (p.new_value.value *= 7);
		});

	x.set_value(test_type(1));

	EXPECT_EQ(old_before, -2);
	EXPECT_EQ(new_before, 3);
	EXPECT_EQ(old_after, -5 * 2);
	EXPECT_EQ(new_after, 7 * 3);

	EXPECT_EQ(x.value().value, 7 * 3);
}
TEST(Listenable, multiple_observer_pair) {
	test_listenable x(-1);
	int old_before1, new_before1, old_after1, new_after1,
		old_before2, new_before2, old_after2, new_after2;

	x.value_updating().subscribe([&old_before1, &new_before1](test_listenable::update_parameters p)
		{
			old_before1 = (p.old_value.value *= 2);
			new_before1 = (p.new_value.value *= 3);
		});
	x.value_updating().subscribe([&old_before2, &new_before2](test_listenable::update_parameters p)
		{
			old_before2 = (p.old_value.value *= 5);
			new_before2 = (p.new_value.value *= 7);
		});
	x.value_updated().subscribe([&old_after1, &new_after1](test_listenable::update_parameters p)
		{
			old_after1 = (p.old_value.value *= 11);
			new_after1 = (p.new_value.value *= 13);
		});
	x.value_updated().subscribe([&old_after2, &new_after2](test_listenable::update_parameters p)
		{
			old_after2 = (p.old_value.value *= 17);
			new_after2 = (p.new_value.value *= 19);
		});

	x.set_value(test_type(1));

	EXPECT_EQ(old_before1, -2);
	EXPECT_EQ(new_before1, 3);
	EXPECT_EQ(old_before2, -5 * 2);
	EXPECT_EQ(new_before2, 7 * 3);

	EXPECT_EQ(old_after1, -11 * 2 * 5);
	EXPECT_EQ(new_after1, 13 * 7 * 3);
	EXPECT_EQ(old_after2, -17 * 11 * 2 * 5);
	EXPECT_EQ(new_after2, 19 * 13 * 7 * 3);

	EXPECT_EQ(x.value().value, 19 * 13 * 7 * 3);
}
