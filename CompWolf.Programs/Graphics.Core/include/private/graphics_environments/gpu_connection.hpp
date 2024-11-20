#ifndef COMPWOLF_GPU_CONNECTION
#define COMPWOLF_GPU_CONNECTION

#include <concepts>

namespace compwolf
{
	/** A boundary class representing one or more GPUs on a machine.
	 * An object of the class must:
	 * * Have a const method "work_types()", which returns a [[gpu_work_type_set]].
	 * @see graphics_environment
	 */
	template<typename Gpu>
	concept GpuConnection = requires(const Gpu const_gpu)
	{
		{ const_gpu.work_types() } -> std::convertible_to<gpu_work_type_set>;
	};
}

#endif // ! COMPWOLF_GPU_CONNECTION