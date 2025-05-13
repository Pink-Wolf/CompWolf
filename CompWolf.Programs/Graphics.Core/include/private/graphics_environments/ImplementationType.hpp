#ifndef COMPWOLF_GRAPHICS_IMPLEMENTATION_TYPE
#define COMPWOLF_GRAPHICS_IMPLEMENTATION_TYPE

#include <utility>

namespace compwolf
{
	/** @hidden */
	struct pixel_output_type;
	/** The different types of buffers, in terms of how they can be used. */
	enum class gpu_buffer_usage
	{
		input,
		input_index,
		field,
	};

	/** A container for the the set of types implementing [[Graphics.Core]]. */
	template <typename T>
	concept ImplementationType = requires (T t)
	{
		/** The implementation of [[graphics_environment]]. */
		typename T::environment;
		/** The implementation of [[gpu_connection]]. */
		typename T::gpu;

		/** The implementation of [[window]]. */
		typename T::window;
		/** The implementation of [[window_camera]]. */
		typename T::camera;

		/** The implementation of [[gpu_buffer]]. */
		typename T::template buffer<gpu_buffer_usage::input, int>;

		/** The implementation of [[shader]]. */
		typename T::template shader<int, int>;

		/** The implementation of [[gpu_fence]]. */
		typename T::fence;
		/** The implementation of [[gpu_specific_program]]. */
		typename T::program;

		/** The implementation of [[brush]]. */
		typename T::template brush<
			typename T::template shader<int, int>,
			typename T::template shader<int, pixel_output_type>
		>;
		/** The implementation of [[drawable]]. */
		typename T::template drawable<typename T::template brush<
			typename T::template shader<int, int>,
			typename T::template shader<int, pixel_output_type>
		>>;
	};

	/** Define this to denote what implementation of [[Graphics.Core]] you want to use by default. */
	struct default_implementation;
}

#endif // ! COMPWOLF_GRAPHICS_IMPLEMENTATION_TYPE
