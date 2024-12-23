#ifndef COMPWOLF_VERSION_NUMBER
#define COMPWOLF_VERSION_NUMBER

namespace compwolf
{
	/** A number denoting the version of a program, split into a major-, minor-, and patch-integer. */
	struct version_number
	{
		/* The major part of the version-number, usually increasing when major, possible breaking changes to behaviour are done. */
		int major;
		/* The minor part of the version-number, usually increasing when minor, non-breaking changes to behaviour are done. */
		int minor;
		/* The patch part of the version-number, usually increasing when bugs are fixed. */
		int patch;
	};

	/** The version of CompWolf the used library is.
	 * @see version_number
	 */
	extern const version_number compwolf_version;
}

#endif // ! COMPWOLF_VERSION_NUMBER
