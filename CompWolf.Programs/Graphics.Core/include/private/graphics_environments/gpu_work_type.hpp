#ifndef COMPWOLF_GPU_WORK_TYPE
#define COMPWOLF_GPU_WORK_TYPE

namespace compwolf
{
	/** A type of work that a GPU may be able to do.
	 * @see gpu_work_type_set
	 */
	enum class gpu_work_type
	{
		/** Drawing objects on an image. */
		draw,
		/** Change what image is shown on a window. */
		present,
		/** The amount of values in this enum, excluding this. */
		size,
	};
}

#endif // ! COMPWOLF_GPU_WORK_TYPE