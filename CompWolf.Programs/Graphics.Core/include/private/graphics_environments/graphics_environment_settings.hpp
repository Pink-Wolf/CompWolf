#ifndef COMPWOLF_GRAPHICS_ENVIRONMENT_SETTINGS
#define COMPWOLF_GRAPHICS_ENVIRONMENT_SETTINGS

#include <string>
#include <version_numbers>
#include <functional>

namespace compwolf
{
	/** Aggregate parameter for [[graphics_environment]]'s constructor.
	 * @see graphics_environment
	 */
	struct graphics_environment_settings
	{
		/** What the environment should report as the name of the program. */
		std::string program_name{};
		/** What the environment should report as the version of the program. */
		version_number program_version{};
	};
}

#endif // ! COMPWOLF_GRAPHICS_ENVIRONMENT_SETTINGS