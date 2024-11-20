#ifndef COMPWOLF_GRAPHICS_ENVIRONMENT
#define COMPWOLF_GRAPHICS_ENVIRONMENT

#include "graphics_environment_settings.hpp"
#include "graphics_boundary_classes.hpp"
#include <freeables>
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
	 * @typeparam GraphicsBoundaryClasses A [[graphics_boundary_classes]] denoting the boundary classes that the environment will use.
	 * @warning It is undefined behaviour to construct or destruct a [[graphics_environment]] on a thread other than the one that started the program.
	 */
	template<GraphicsBoundaryClasses BoundaryCollection>
	class graphics_environment : public basic_freeable
	{
	public:
		/** The boundary classes used by this [[graphics_environment]]. */
		using boundary_classes = BoundaryCollection;

	private:
		graphics_environment_settings _settings;
		std::thread::id _main_graphics_thread;

	public: // constructors
		/** @overload Constructs a freed [[graphics_environment]].
		 * See [[freeable]] for more information.
		 * @see freeable
		 */
		graphics_environment() noexcept = default;
		/**
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @warning It is undefined behaviour to construct or destruct a [[graphics_environment]] on a thread other than the one that started the program.
		 * @see graphics_environment_settings
		 */
		graphics_environment(graphics_environment_settings settings)
			: _settings(settings)
			, _main_graphics_thread(std::this_thread::get_id())
		{}

	public: // accessors
		/** Returns whether the given thread is the main graphics thread.
		 * @see graphics_environment::is_this_main_thread
		 */
		auto is_main_thread(std::thread::id id) const noexcept -> bool
		{
			return id == _main_graphics_thread;
		}
		/** Returns whether the thread calling this is the main graphics thread.
		 * @see graphics_environment::is_main_thread
		 */
		auto is_this_main_thread() const noexcept -> bool
		{
			return is_main_thread(std::this_thread::get_id());
		}

		/** Returns the [[graphics_environment_settings]] passed to the [[graphics_environment]]'s constructor.
		 * @see graphics_environment_settings
		 */
		auto settings() const noexcept -> const graphics_environment_settings&
		{
			return _settings;
		}

		/** Returns the GPUs that the environment can use.
		 * An item in this array may be representing multiple actual GPUs.
		 */
		virtual auto gpus() const noexcept -> const std::vector<typename boundary_classes::gpu_type>& = 0;

	public: // update
		/** Handles any jobs from outside the program, which has been received since the last call to [[graphics_environment]]::update.
		 * Jobs includes for example updating what keyboard keys are being pressed.
		 * 
		 * Must be called from the main graphics thread.
		 * @throws any exception thrown by code handling the jobs.
		 */
		virtual void update() = 0;

	public: // compwolf::freeable
		/** @see freeable */
		auto empty() const noexcept -> bool final
		{
			return _main_graphics_thread == std::thread::id();
		}
		/** @see freeable */
		void free() noexcept final
		{
			if (empty()) return;

			freeing();
			on_free();
			_main_graphics_thread = std::thread::id();
			freed();
		}
	protected:
		virtual void on_free() noexcept = 0;

	public: // events
		/** Invoked before the environment is freed. */
		event<> freeing;
		/** Invoked after the environment has been freed. */
		event<> freed;
	};
}

#endif // ! COMPWOLF_GRAPHICS_ENVIRONMENT