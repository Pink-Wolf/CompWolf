#ifndef COMPWOLF_GRAPHICS_WINDOW
#define COMPWOLF_GRAPHICS_WINDOW

#include "window_settings.hpp"
#include <dimensions>
#include <events>

namespace compwolf
{
	/** Boundary class representing a window on a screen.
	 * That is, a rectangle one can draw on, and which can receive events like keyboard inputs.
	 * 
	 * This class is abstract; a derived type should generally implement/override:
	 * * [[window::update_image]]
	 * * [[window::close]]
	 * 
	 * @typeparam GraphicsEnvironmentType The type of [[graphics_environment]] that this window works with.
	 */
	template <typename GraphicsEnvironmentType>
	class window
	{
	public:
		/** The type of [[graphics_environment]] that this window works with. */
		using environment_type = GraphicsEnvironmentType;

	private:
		environment_type::gpu_type* _gpu{};

		bool _running{};
		event<> _closing{};

		listenable<int2> _pixel_size{};
		event<void> _image_updating{};

	public: // accessors
		/** Returns the gpu that the window is on.
		 * @customoverload
		 */
		auto gpu() const noexcept -> const typename environment_type::gpu_type& { return *_gpu; };
		/** Returns the gpu that the window is on. */
		auto gpu() noexcept -> typename environment_type::gpu_type& { return *_gpu; };

		/** Returns the width and height of the window, in pixels.
		 * This size does not include any border around the window.
		 */
		auto pixel_size() const noexcept -> int2 { return _pixel_size.value(); }
		/** Returns the width and height of the window, in pixels.
		 * This size does not include any border around the window.
		 */
		auto pixel_size_listenable() const noexcept -> listenable<int2> { return _pixel_size; }

		/** Returns whether the window is up and running and not closed. */
		auto running() const noexcept -> bool { return _running; }
		/** Returns whether the window is up and running and not closed. */
		operator bool() const noexcept
		{
			return running();
		}

		/** Returns an event that is invoked right before the window is closed. */
		auto closing() const noexcept -> const event<>& { return _closing; }
		/** Returns an event that is invoked right before the window is closed. */
		auto destructing() const noexcept -> const event<>& { return _closing; }

		/** Returns an event that is invoked right before the window's image is being updated. */
		auto image_updating() const noexcept -> const event<void>& { return _image_updating; }

	public: // modifiers
		/** Makes the window update what is shown on it. */
		virtual void update_image()
		{
			_image_updating.invoke();
		}

		/** Closes the window. */
		virtual void close() noexcept
		{
			_closing.invoke();
			_running = false;
		}

	protected:
		/** Sets what gpu the window reports it is on;
		 * @warning This does not actually change what gpu the window is actually on.
		 */
		auto set_gpu(environment_type::gpu_type* gpu) noexcept -> environment_type::gpu_type*
		{
			_gpu = gpu;
			return _gpu;
		}

	public: // constructors
		/** Constructs an invalid [[window]].
		 * Using this window is undefined behaviour.
		 * @overload
		 */
		window() = default;
		window(window&&) = default;
		auto operator=(window&&) -> window& = default;

	private:
		window(environment_type::gpu_type* gpu, window_settings& settings) noexcept
		{
			if (settings.pixel_size.x() <= 0 || settings.pixel_size.y() <= 0)
				settings.pixel_size = int2({ 480, 480 });

			if (settings.name.empty())
				settings.name = "Window";

			_running = true;
			_gpu = nullptr;
			set_gpu(gpu);
			_pixel_size = settings.pixel_size;

		}
	public:
		/** Constructs a window on no specific gpu, with the given settings.
		 * Also sets invalid values of settings to some default value
		 * [[window::set_gpu]] must be called afterwards to set the gpu!
		 * @overload sets the window to not have a gpu.
		 * [[window::set_gpu]] must be called afterwards to set the gpu!
		 */
		window(environment_type&, window_settings& settings) noexcept
			: window(nullptr, settings) {}

		/** Constructs a window on the given gpu, with the given settings.
		 * Also sets invalid values of settings to some default value
		 */
		window(environment_type::gpu_type& gpu, window_settings& settings) noexcept
			: window(&gpu, settings) {}
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW
