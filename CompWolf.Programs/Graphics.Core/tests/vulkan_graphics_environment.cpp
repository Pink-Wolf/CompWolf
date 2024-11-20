#include <gtest/gtest.h>
#include <vulkan_graphics_environments>
#include <private/vulkan_graphics_environments/vulkan_graphics_environment.hpp>

TEST(VulkanGraphicsEnvironment, construction_and_destruction) {
	ASSERT_NO_THROW({
		compwolf::vulkan::vulkan_graphics_environment_settings settings;
		compwolf::vulkan::vulkan_graphics_environment e(settings);
		});
}
TEST(VulkanGraphicsEnvironment, multiple_constructions) {
	EXPECT_NO_THROW({
		compwolf::vulkan::vulkan_graphics_environment_settings settings;
		compwolf::vulkan::vulkan_graphics_environment e1(settings);
		compwolf::vulkan::vulkan_graphics_environment e2(settings);
		compwolf::vulkan::vulkan_graphics_environment e3(settings);
		});
}
TEST(VulkanGraphicsEnvironment, multiple_constructions_unordered_destruction) {
	EXPECT_NO_THROW({
		compwolf::vulkan::vulkan_graphics_environment_settings settings;
		compwolf::vulkan::vulkan_graphics_environment e1(settings);
		compwolf::vulkan::vulkan_graphics_environment e2(settings);
		compwolf::vulkan::vulkan_graphics_environment e3(settings);
		e2.free();
		e1.free();
		e3.free();
		});
}
TEST(VulkanGraphicsEnvironment, reconstruction) {
	EXPECT_NO_THROW({
		for (size_t i = 0; i < 4; i++)
		{
			compwolf::vulkan::vulkan_graphics_environment_settings settings;
			compwolf::vulkan::vulkan_graphics_environment e(settings);
		}
		});
}
TEST(VulkanGraphicsEnvironment, construction_with_debugger) {
	ASSERT_NO_THROW({
		compwolf::vulkan::vulkan_graphics_environment_settings settings{
			.internal_debug_callback = [](std::string_view) {}
		};
		compwolf::vulkan::vulkan_graphics_environment e(settings);
		});
}
