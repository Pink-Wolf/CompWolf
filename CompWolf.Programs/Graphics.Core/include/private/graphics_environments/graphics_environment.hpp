#ifndef COMPWOLF_GRAPHICS_ENVIRONMENT
#define COMPWOLF_GRAPHICS_ENVIRONMENT

#include "graphics_environment_settings.hpp"
#include "input_state.hpp"
#include <events>
#include <thread>
#include <vector>

namespace compwolf
{
	/** Base class for setting up GPU-logic, which is required by most other logic in the library.
	 * 
	 * This class is abstract; its pure virtual functions are:
	 * * [[graphics_environment::gpus]],
	 * * [[graphics_environment::update]],
	 * * [[graphics_environment::on_free]].
	 * Furthermore, remember to make destructors call [[freeable::free]].
	 * 
	 * The term "main graphics thread" refers to the thread this was constructed on.
	 * @typeparam GpuType The boundary class representing one or more GPUs on a machine.
	 * @warning It is undefined behaviour to construct or destruct a [[graphics_environment]] on a thread other than the one that started the program.
	 */
	template<GpuConnection GpuType>
	class graphics_environment
	{
	public:
		/** The boundary class representing one or more GPUs on a machine. */
		using gpu_type = GpuType;

	private:
		destruct_event<> _destructing;
		graphics_environment_settings _settings;
		input_state _inputs;
		std::thread::id _main_graphics_thread;

	public: // constructors
		/** Constructs an invalid [[graphics_environment]].
		 * Using this environment, except [[graphics_environment::operator bool]], is undefined behaviour.
		 * @overload
		 */
		graphics_environment() noexcept = default;
		graphics_environment(graphics_environment&&) = default;
		auto operator=(graphics_environment&&) -> graphics_environment& = default;
		/**
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @warning It is undefined behaviour to construct or destruct a [[graphics_environment]] on a thread other than the one that started the program.
		 * @see graphics_environment_settings
		 */
		graphics_environment(graphics_environment_settings settings)
			: _settings(settings)
			, _inputs()
			, _main_graphics_thread(std::this_thread::get_id())
		{}

	public: // accessors
		/** Returns whether the given thread is the main graphics thread.
		 * @see graphics_environment::is_this_main_thread
		 */
		auto is_main_thread(std::thread::id id) const noexcept -> bool
			{ return id == _main_graphics_thread; }
		/** Returns whether the thread calling this is the main graphics thread.
		 * @see graphics_environment::is_main_thread
		 */
		auto is_this_main_thread() const noexcept -> bool
			{ return is_main_thread(std::this_thread::get_id()); }

		/** Returns the [[graphics_environment_settings]] passed to the [[graphics_environment]]'s constructor.
		 * @see graphics_environment_settings
		 */
		auto settings() const noexcept -> const graphics_environment_settings&
			{ return _settings; }

		/** Returns the [[input_state]] denoting the inputs that the environment is currently receiving.
		 * @see input_state
		 */
		auto inputs() const noexcept -> const input_state&
			{ return _inputs; }
		/** Returns the [[input_state]] denoting the inputs that the environment is currently receiving.
		 * @see input_state
		 */
		auto inputs() noexcept -> input_state&
			{ return _inputs; }

		/** Returns the GPUs that the environment can use.
		 * An item in this array may be representing multiple actual GPUs.
		 */
		virtual auto gpus() const noexcept -> const std::vector<typename gpu_type>& = 0;

		/** Returns an event that is invoked right before the environment is destructed. */
		auto destructing() const noexcept -> const event<>&
			{ return _destructing; }

		/** Returns whether this is valid, that is one not constructed by the default constructor. */
		operator bool() const noexcept
			{ return _main_graphics_thread == std::thread::id(); }

	public: // modifiers
		/** Handles any jobs from outside the program, which has been received since the last call to [[graphics_environment]]::update.
		 * Jobs includes for example updating what keyboard keys are being pressed.
		 * 
		 * Must be called from the main graphics thread.
		 * @throws any exception thrown by code handling the jobs.
		 */
		virtual void update()
		{
			inputs().update_last_frame_data();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_ENVIRONMENT