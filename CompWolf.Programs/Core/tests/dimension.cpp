#pragma warning(push, 0)
#include <gtest/gtest.h>
#pragma warning(pop)
#include <dimensions>

//Assert types exists:
compwolf::float2 f2; compwolf::float3 f3; compwolf::float4 f4;
compwolf::float2x2 f22; compwolf::float3x2 f32; compwolf::float4x2 f42;
compwolf::float2x3 f23; compwolf::float3x3 f33; compwolf::float4x3 f43;
compwolf::float2x4 f24; compwolf::float3x4 f34; compwolf::float4x4 f44;
compwolf::double2 d2; compwolf::double3 d3; compwolf::double4 d4;
compwolf::double2x2 d22; compwolf::double3x2 d32; compwolf::double4x2 d42;
compwolf::double2x3 d23; compwolf::double3x3 d33; compwolf::double4x3 d43;
compwolf::double2x4 d24; compwolf::double3x4 d34; compwolf::double4x4 d44;
compwolf::int2 i2; compwolf::int3 i3; compwolf::int4 i4;
compwolf::int2x2 i22; compwolf::int3x2 i32; compwolf::int4x2 i42;
compwolf::int2x3 i23; compwolf::int3x3 i33; compwolf::int4x3 i43;
compwolf::int2x4 i24; compwolf::int3x4 i34; compwolf::int4x4 i44;
compwolf::bool2 b2; compwolf::bool3 b3; compwolf::bool4 b4;
compwolf::bool2x2 b22; compwolf::bool3x2 b32; compwolf::bool4x2 b42;
compwolf::bool2x3 b23; compwolf::bool3x3 b33; compwolf::bool4x3 b43;
compwolf::bool2x4 b24; compwolf::bool3x4 b34; compwolf::bool4x4 b44;

TEST(Dimension, construct_initializer_list_5x1) {
	compwolf::dimensional_array<int, 5> x({ 0, 1, 2, 3, 4 });

	EXPECT_EQ(x[0], 0);
	EXPECT_EQ(x[1], 1);
	EXPECT_EQ(x[2], 2);
	EXPECT_EQ(x[3], 3);
	EXPECT_EQ(x[4], 4);
}
TEST(Dimension, construct_short_initializer_list_5x1) {
	compwolf::dimensional_array<int, 5> x({ 0, 1, 2 });

	EXPECT_EQ(x[0], 0);
	EXPECT_EQ(x[1], 1);
	EXPECT_EQ(x[2], 2);
	EXPECT_EQ(x[3], 0);
	EXPECT_EQ(x[4], 0);
}
TEST(Dimension, construct_initializer_list_3x3) {
	compwolf::dimensional_array<int, 3, 3> x({ 0, 1, 2, 3, 4, 5, 6, 7, 8 });

	EXPECT_EQ(x[0], 0);
	EXPECT_EQ(x[1], 1);
	EXPECT_EQ(x[2], 2);
	EXPECT_EQ(x[3], 3);
	EXPECT_EQ(x[4], 4);
	EXPECT_EQ(x[5], 5);
	EXPECT_EQ(x[6], 6);
	EXPECT_EQ(x[7], 7);
	EXPECT_EQ(x[8], 8);
}
TEST(Dimension, construct_short_initializer_list_3x3) {
	compwolf::dimensional_array<int, 3, 3> x({ 0, 1, 2, 3, 4 });

	EXPECT_EQ(x[0], 0);
	EXPECT_EQ(x[1], 1);
	EXPECT_EQ(x[2], 2);
	EXPECT_EQ(x[3], 3);
	EXPECT_EQ(x[4], 4);
	EXPECT_EQ(x[5], 0);
	EXPECT_EQ(x[6], 0);
	EXPECT_EQ(x[7], 0);
	EXPECT_EQ(x[8], 0);
}

TEST(Dimension, x_1) {
	compwolf::dimensional_array<int, 1> x({ 1 });
	ASSERT_EQ(x.x(), 1);
}
TEST(Dimension, x_6) {
	compwolf::dimensional_array<int, 6> x({ 1, 2, 3, 4, 5, 6 });
	ASSERT_EQ(x.x(), 1);
}
TEST(Dimension, x_3x2) {
	compwolf::dimensional_array<int, 3, 2> x({ 1, 2, 3, 4, 5, 6 });
	ASSERT_EQ(x.x(), 1);
}

TEST(Dimension, y_2) {
	compwolf::dimensional_array<int, 2> x({ 1, 2 });
	ASSERT_EQ(x.y(), 2);
}
TEST(Dimension, y_6) {
	compwolf::dimensional_array<int, 6> x({ 1, 2, 3, 4, 5, 6 });
	ASSERT_EQ(x.y(), 2);
}
TEST(Dimension, y_3x2) {
	compwolf::dimensional_array<int, 3, 2> x({ 1, 2, 3, 4, 5, 6 });
	ASSERT_EQ(x.y(), 2);
}

TEST(Dimension, z_3) {
	compwolf::dimensional_array<int, 3> x({ 1, 2, 3 });
	ASSERT_EQ(x.z(), 3);
}
TEST(Dimension, z_6) {
	compwolf::dimensional_array<int, 6> x({ 1, 2, 3, 4, 5, 6 });
	ASSERT_EQ(x.z(), 3);
}
TEST(Dimension, z_3x2) {
	compwolf::dimensional_array<int, 3, 2> x({ 1, 2, 3, 4, 5, 6 });
	ASSERT_EQ(x.z(), 3);
}

TEST(Dimension, w_4) {
	compwolf::dimensional_array<int, 4> x({ 1, 2, 3, 4 });
	ASSERT_EQ(x.w(), 4);
}
TEST(Dimension, w_6) {
	compwolf::dimensional_array<int, 6> x({ 1, 2, 3, 4, 5, 6 });
	ASSERT_EQ(x.w(), 4);
}
TEST(Dimension, w_4x2) {
	compwolf::dimensional_array<int, 4, 2> x({ 1, 2, 3, 4, 5, 6, 7, 8 });
	ASSERT_EQ(x.w(), 4);
}


TEST(Dimension, xx_1x1) {
	compwolf::dimensional_array<int, 1, 1> x({ 1 });
	ASSERT_EQ(x.xx(), 1);
}
TEST(Dimension, xx_3x3) {
	compwolf::dimensional_array<int, 3, 3> x({ 1, 2, 3, 4, 5, 6, 7, 8, 9 });
	ASSERT_EQ(x.xx(), 1);
}

TEST(Dimension, yz_2x3) {
	compwolf::dimensional_array<int, 2, 3> x({ 11, 21, 12, 22, 13, 23 });
	ASSERT_EQ(x.yz(), 23);
}
TEST(Dimension, yz_3x3) {
	compwolf::dimensional_array<int, 3, 3> x({ 11, 21, 31, 12, 22, 32, 13, 23, 33 });
	ASSERT_EQ(x.yz(), 23);
}

TEST(Dimension, at1_2x1) {
	compwolf::dimensional_array<int, 2, 1> x({ 0, 1 });
	ASSERT_EQ(x.at({ 1 }), 1);
}
TEST(Dimension, at1_2x3) {
	compwolf::dimensional_array<int, 2, 3> x({ 00, 10, 01, 11, 02, 12 });
	ASSERT_EQ(x.at({ 1 }), 10);
}
TEST(Dimension, at12_2x3) {
	compwolf::dimensional_array<int, 2, 3> x({ 00, 10, 01, 11, 02, 12 });
	ASSERT_EQ(x.at({ 1, 2 }), 12);
}
TEST(Dimension, at12_3x3) {
	compwolf::dimensional_array<int, 3, 3> x({ 00, 10, 20, 01, 11, 21, 02, 12, 22 });
	ASSERT_EQ(x.at({ 1, 2 }), 12);
}

TEST(Dimension, size_in_dimension0_1) {
	compwolf::dimensional_array<int, 1> x;
	ASSERT_EQ(x.size_in_dimension(0), 1);
}
TEST(Dimension, size_in_dimension0_3) {
	compwolf::dimensional_array<int, 3> x;
	ASSERT_EQ(x.size_in_dimension(0), 3);
}
TEST(Dimension, size_in_dimension0_1x5) {
	compwolf::dimensional_array<int, 1, 5> x;
	ASSERT_EQ(x.size_in_dimension(0), 1);
}
TEST(Dimension, size_in_dimension0_3x5) {
	compwolf::dimensional_array<int, 3, 5> x;
	ASSERT_EQ(x.size_in_dimension(0), 3);
}
TEST(Dimension, size_in_dimension1_5x1) {
	compwolf::dimensional_array<int, 5, 1> x;
	ASSERT_EQ(x.size_in_dimension(1), 1);
}
TEST(Dimension, size_in_dimension1_5x3) {
	compwolf::dimensional_array<int, 5, 3> x;
	ASSERT_EQ(x.size_in_dimension(1), 3);
}
TEST(Dimension, size_in_dimension1_5x1x5) {
	compwolf::dimensional_array<int, 5, 1, 5> x;
	ASSERT_EQ(x.size_in_dimension(1), 1);
}
TEST(Dimension, size_in_dimension1_5x3x5) {
	compwolf::dimensional_array<int, 5, 3, 5> x;
	ASSERT_EQ(x.size_in_dimension(1), 3);
}
