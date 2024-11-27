#ifndef COMPWOLF_GPU_CONNECTION
#define COMPWOLF_GPU_CONNECTION

#include "graphics_environment.hpp"

namespace compwolf
{
	/** A boundary class representing one or more GPUs on a machine.
	 * @typeparam EnvironmentType The type of environment that the gpu is on.
	 * @see graphics_environment
	 */
	template <typename EnvironmentType>
	class gpu_connection
	{
	public:
		/** The type of environment that the gpu is on. */
		using environment_type = EnvironmentType;
	private:
		environment_type* _environment;

		gpu_work_type_set _work_types{};

	public: // accessors
		/** Returns the environment that the gpu is on.
		 * @see graphics_environment
		 * @uniqueoverload
		 */
		auto environment() const noexcept -> const environment_type&
			{ return *_environment; }

		/** Returns the environment that the gpu is on.
		 * @see graphics_environment
		 */
		auto environment() noexcept -> environment_type&
			{ return *_environment; }

		/** Returns the type of work that the gpu can do.
		 * @see gpu_work_type_set
		 * @uniqueoverload
		 */
		auto work_types() const noexcept -> gpu_work_type_set
			{ return _work_types; }
	protected:
		/** Returns the type of work that the gpu can do.
		 * @see gpu_work_type_set
		 */
		auto work_types() noexcept -> gpu_work_type_set&
			{ return _work_types; }
	public:

		/** Returns an event that is invoked right before the environment is destructed. */
		auto destructing() const noexcept -> const event<>&
		{ return environment().destructing(); }

	public: // constructors
		/** Constructs an invalid [[gpu_connection]].
		 * Using this is undefined behaviour.
		 * @overload
		 */
		gpu_connection() noexcept = default;
		gpu_connection(gpu_connection&&) = default;
		auto operator=(gpu_connection&&) -> gpu_connection& = default;

		/** Should be called by [[graphics_environment]].
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @see graphics_environment
		 */
		gpu_connection(environment_type& environment, gpu_work_type_set work_types)
			: _environment(&environment), _work_types(work_types)
		{}
	};
}

#endif // ! COMPWOLF_GPU_CONNECTION