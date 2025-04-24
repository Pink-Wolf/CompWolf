#include <gtest/gtest.h>
#include <gpu_structs>
#include <dimensions>

struct vertex
{
	compwolf::transparent_color color;
	compwolf::float3 pos;
};
template<> struct compwolf::gpu_struct_info<vertex> : public compwolf::new_gpu_struct_info<
	compwolf::type_value_pair<compwolf::transparent_color, offsetof(vertex, color)>,
	compwolf::type_value_pair<compwolf::float3, offsetof(vertex, pos)>
> {};
using vertex_primitives = compwolf::gpu_struct_info<vertex>::primitives;

TEST(NewGPUStructInfo, simple) {
	EXPECT_EQ((vertex_primitives::at<0>::type()), compwolf::transparent_color());
	EXPECT_EQ((vertex_primitives::at<1>::type()), compwolf::float3());

	EXPECT_EQ((vertex_primitives::at<0>::value), 0);
	EXPECT_EQ((vertex_primitives::at<1>::value), sizeof(compwolf::transparent_color));

	EXPECT_EQ((vertex_primitives::size), std::size_t(2));
}

struct composition
{
	vertex v;
	compwolf::shader_int value;
};
template<> struct compwolf::gpu_struct_info<composition> : public compwolf::new_gpu_struct_info<
	compwolf::type_value_pair<vertex, offsetof(composition, v)>,
	compwolf::type_value_pair<compwolf::shader_int, offsetof(composition, value)>
> {
};
using composition_primitives = compwolf::gpu_struct_info<composition>::primitives;

TEST(NewGPUStructInfo, composition) {
	EXPECT_EQ((composition_primitives::at<0>::type()), compwolf::transparent_color());
	EXPECT_EQ((composition_primitives::at<1>::type()), compwolf::float3());
	EXPECT_EQ((composition_primitives::at<2>::type()), compwolf::shader_int());

	EXPECT_EQ((composition_primitives::at<0>::value), 0);
	EXPECT_EQ((composition_primitives::at<1>::value), sizeof(compwolf::transparent_color));
	EXPECT_EQ((composition_primitives::at<2>::value), sizeof(compwolf::transparent_color) + sizeof(compwolf::float3));

	EXPECT_EQ((composition_primitives::size), std::size_t(3));
}
