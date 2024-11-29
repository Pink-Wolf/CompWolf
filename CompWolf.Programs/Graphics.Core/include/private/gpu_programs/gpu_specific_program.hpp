#ifndef COMPWOLF_GRAPHICS_GPU_SPECIFIC_PROGRAM
#define COMPWOLF_GRAPHICS_GPU_SPECIFIC_PROGRAM

#include <graphics_environments>

namespace compwolf
{
	/** Boundary class representing logic that can be performed on a gpu.
	 * 
	 * This class is abstract; a derived type should generally implement/override:
	 * * [[window::execute]]
	 * 
	 * @typeparam GraphicsEnvironmentType The type of [[graphics_environment]] that this program works with.
	 */
	template <typename GraphicsEnvironmentType, typename FenceType>
	class gpu_specific_program
	{
	public:
		/** The type of [[graphics_environment]] that this program works with. */
		using environment_type = GraphicsEnvironmentType;

		/** The type of [[gpu_fence]] that this program works with. */
		using fence_type = FenceType;

	private:
		environment_type::gpu_type* _gpu;

	public: // accessors
		/** Returns the gpu that the program is on.
		 * @uniqueoverload
		 */
		auto gpu() noexcept -> environment_type::gpu_type& { return *_gpu; }
		/** Returns the gpu that the program is on. */
		auto gpu() const noexcept -> const environment_type::gpu_type& { return *_gpu; }

	public: // modifiers
		/** Runs the program.
		 * @return a fence denoting when the program is finished running.
		 */
		virtual auto execute() -> const fence_type& = 0;

	public: // constructors
		/** Constructs an invalid [[window]].
		 * Using this window is undefined behaviour.
		 * @overload
		 */
		gpu_specific_program() = default;
		gpu_specific_program(gpu_specific_program&&) = default;
		auto operator=(gpu_specific_program&&) -> gpu_specific_program& = default;

		/** Creates a program on the given gpu. */
		gpu_specific_program(environment_type::gpu_type& gpu) : _gpu(&gpu)
		{}
	};
}

#endif // ! COMPWOLF_GRAPHICS_GPU_SPECIFIC_PROGRAM
