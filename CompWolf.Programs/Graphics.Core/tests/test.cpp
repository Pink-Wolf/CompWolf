#include <gtest/gtest.h>
#include <version_number>

TEST(CompwolfVersion2, major) {
	EXPECT_EQ(CompWolf::compwolf_version.major, 1);
}
TEST(CompwolfVersion2, minor) {
	EXPECT_EQ(CompWolf::compwolf_version.minor, 2);
}
TEST(CompwolfVersion2, patch) {
	EXPECT_EQ(CompWolf::compwolf_version.patch, 3);
}
