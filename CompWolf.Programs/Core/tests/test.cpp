#include <gtest/gtest.h>
#include <version_number>

TEST(CompwolfVersion, major) {
	EXPECT_EQ(CompWolf::compwolf_version.major, 1);
}
TEST(CompwolfVersion, minor) {
	EXPECT_EQ(CompWolf::compwolf_version.minor, 2);
}
TEST(CompwolfVersion, patch) {
	EXPECT_EQ(CompWolf::compwolf_version.patch, 3);
}
