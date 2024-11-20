#include <gtest/gtest.h>
#include <events>

TEST(Event, empty_invoke) {
	compwolf::event<> e;
	e.invoke();
	e();
}
TEST(Event, invoke_method) {
	compwolf::event<> e;
	int i = 0;
	e.subscribe([&i]() { ++i; });

	e.invoke();

	ASSERT_EQ(i, 1);
}
TEST(Event, invoke) {
	compwolf::event<> e;
	int i = 0;
	e.subscribe([&i]() { ++i; });

	e();

	ASSERT_EQ(i, 1);
}
TEST(Event, multiple_invokes) {
	compwolf::event<> e;
	int i = 0;
	e.subscribe([&i]() { ++i; });

	e();
	e();
	e();

	ASSERT_EQ(i, 3);
}
TEST(Event, invoke_multiple_subscribers) {
	compwolf::event<> e;
	int i = 0;
	e.subscribe([&i]() { i += 100; });
	e();
	e.subscribe([&i]() { i += 10; });
	e();
	e.subscribe([&i]() { i += 1; });
	e();

	ASSERT_EQ(i, 321);
}
TEST(Event, invoke_after_unsubscribing) {
	compwolf::event<> e;
	int i = 0;
	auto k = e.subscribe([&i]() { ++i; });
	e();
	e.unsubscribe(k);
	e();

	ASSERT_EQ(i, 1);
}
TEST(Event, unsubscribe_during_invoke) {
	compwolf::event<> e;
	int i = 0;
	compwolf::event<>::key_type k;
	k = e.subscribe([&e, &i, &k]() { ++i; e.unsubscribe(k); });
	e();
	e();
	e();

	ASSERT_EQ(i, 1);
}
TEST(Event, unsubscribe_multiple_during_invoke) {
	compwolf::event<> e;
	int i1, i2, i3, i4, i5;
	i1 = i2 = i3 = i4 = i5 = 0;
	compwolf::event<>::key_type k1, k2, k3, k4, k5;
	k1 = e.subscribe([&e, &i1, &k1]() { i1 += 1; if (i1 >= 1) e.unsubscribe(k1); });
	k2 = e.subscribe([&e, &i2, &k2]() { i2 += 1; if (i2 >= 3) e.unsubscribe(k2); });
	k3 = e.subscribe([&e, &i3, &k3]() { i3 += 1; if (i3 >= 3) e.unsubscribe(k3); });
	k4 = e.subscribe([&e, &i4, &k4]() { i4 += 1; if (i4 >= 8) e.unsubscribe(k4); });
	k5 = e.subscribe([&e, &i5, &k5]() { i5 += 1; if (i5 >= 5) e.unsubscribe(k5); });
	for (size_t i = 0; i < 10; ++i)
	{
		e();
	}

	int i = i1 * 1 + i2 * 10 + i3 * 100 + i4 * 1000 + i5 * 10000;
	ASSERT_EQ(i, 58331);
}
TEST(Event, parameter_invokes) {
	compwolf::event<int> e;
	int i = 0;
	e.subscribe([&i](int j) { i += j; });

	e(1);
	e(10);
	e(100);

	ASSERT_EQ(i, 111);
}
TEST(Event, reference_parameter_invokes) {
	compwolf::event<int&> e;
	int i = 0;
	e.subscribe([&i](int& j) { i += j; j *= 2; });
	e.subscribe([&i](int& j) { i += j; j *= 2; });

	int param = 1;
	e(param); // 1 + 2 =  3
	param = 10;
	e(param); // 1 + 2 =  3
	param = 100;
	e(param); // 1 + 2 =  3
	e(param); // 4 + 8 = 12
			  //            3 + 12 = 15

	ASSERT_EQ(1533, i);
}
