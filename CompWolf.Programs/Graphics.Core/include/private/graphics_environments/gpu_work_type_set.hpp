#ifndef COMPWOLF_GPU_WORK_TYPE_SET
#define COMPWOLF_GPU_WORK_TYPE_SET

#include <enum_bitsets>
#include "gpu_work_type.hpp"

namespace compwolf
{
	/** A set of compwolf::gpu_work_type, as in some types of work that a GPU may be able to do.
	 * @see enum_bitset
	 * @see gpu_work_type
	 */
	using gpu_work_type_set = enum_bitset<gpu_work_type>;
}

#endif // ! COMPWOLF_GPU_WORK_TYPE_SET