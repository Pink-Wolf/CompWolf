#ifndef COMPWOLF_GRAPHICS_BOUNDARY_CLASSES
#define COMPWOLF_GRAPHICS_BOUNDARY_CLASSES

#include "gpu_connection.hpp"

namespace compwolf
{
	/** Contains the boundary classes used to communicate with the GPU in a [[graphics_environment]].
	 * @typeparam Gpu The boundary class representing one or more GPUs on a machine.
	 * @typeparam Shader The boundary class representing some code on a GPU.
	 * @typeparam Buffer The boundary class representing some memory on a GPU.
	 * @typeparam Brush The boundary class representing a graphics pipeline.
	 * That is, represents a collection of shaders used to draw an object with.
	 * @typeparam Window The boundary class representing a window on a screen.
	 * @see graphics_environment
	 */
	template<GpuConnection Gpu,
		typename Shader,
		typename Buffer,
		typename Brush,
		typename Window>
	struct graphics_boundary_classes
	{
		/** The boundary class representing one or more GPUs on a machine. */
		using gpu_type = Gpu;
		/** The boundary class representing some code on a GPU. */
		using shader_type = Shader;
		/** The boundary class representing some memory on a GPU. */
		using buffer_type = Buffer;
		/** The boundary class representing a graphics pipeline.
		 * That is, represents a collection of shaders used to draw an object with.
		 */
		using brush_type = Brush;
		/** The boundary class representing a window on a screen. */
		using window_type = Window;
	};

	/** Concept version of [[graphics_boundary_classes]].
	 * @see graphics_boundary_classes
	 */
	template<typename T>
	concept GraphicsBoundaryClasses = requires
	{
		GpuConnection<typename T::gpu_type>;
		typename T::shader_type;
		typename T::buffer_type;
		typename T::brush_type;
		typename T::window_type;
	};
}

#endif // ! COMPWOLF_GRAPHICS_BOUNDARY_CLASSES