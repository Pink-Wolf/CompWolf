#ifndef COMPWOLF_VERSION_NUMBER_HEADER
#define COMPWOLF_VERSION_NUMBER_HEADER

namespace CompWolf
{
	/* A number denoting the version of a program, split into a major-, minor-, and patch-integer. */
	struct version_number
	{
		/* The major part of the version-number, usually increasing when major, possible breaking changes to behaviour are done. */
		int major;
		/* The minor part of the version-number, usually increasing when minor, non-breaking changes to behaviour are done. */
		int minor;
		/* The patch part of the version-number, usually increasing when bugs are fixed. */
		int patch;
	};

	/* The version of CompWolf the used library is. */
	extern const version_number compwolf_version;
}

#endif // ! COMPWOLF_VERSION_NUMBER_HEADER
