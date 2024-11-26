#ifndef COMPWOLF_GRAPHICS_WINDOW
#define COMPWOLF_GRAPHICS_WINDOW

#include "window_settings.hpp"
#include <freeables>
#include <dimensions>
#include <events>

namespace compwolf
{
	/** Boundary class representing a window on a screen.
	 * That is, a rectangle one can draw on, and which can receive events like keyboard inputs.
	 * 
	 * This class is abstract; its pure virtual functions are:
	 * * [[graphics_environment::update_image]],
	 * * [[graphics_environment::on_free]].
	 * Furthermore, remember to make destructors call [[freeable::free]].
	 * 
	 * @typeparam GraphicsEnvironmentType The type of [[graphics_environment]] that this window works with.
	 */
	template <typename GraphicsEnvironmentType>
	class window : public basic_freeable
	{
	public:
		/** The type of [[graphics_environment]] that this window works with */
		using environment_type = GraphicsEnvironmentType;

	private:
		const GraphicsEnvironmentType::gpu_type* _gpu;
		listenable<int2> _pixel_size;

	public: // accessors
		/** Returns the gpu that the window is on. */
		auto gpu() const noexcept -> const GraphicsEnvironmentType::gpu_type& { return *_gpu; };

		/** Returns the width and height of the window, in pixels.
		 * This size does not include any border around the window.
		 */
		auto pixel_size() const noexcept -> int2 { return _pixel_size.value(); }
		/** Returns the width and height of the window, in pixels.
		 * This size does not include any border around the window.
		 */
		auto pixel_size_listenable() const noexcept -> listenable<int2> { return _pixel_size; }

		/** Returns whether the window is not freed. */
		auto running() const noexcept -> bool { return !empty(); }

	private: // events
		event<void> _image_updating;
		event<void> _image_updated;
	public:
		auto image_updating() const noexcept -> event<void> { return _image_updating; }
		auto image_updated() const noexcept -> event<void> { return _image_updated; }
	protected:
		auto image_updating() noexcept -> event<void> { return _image_updating; }
		auto image_updated() noexcept -> event<void> { return _image_updated; }

	public: // modifiers
		/** Makes the window update what is shown on it. */
		virtual void update_image() = 0;

		/** Frees the window. */
		void close() noexcept { free(); }

	protected:
		/** Sets what gpu the window reports it is on;
		 * @warning This does not actually change what gpu the window is actually on.
		 */
		auto set_gpu(const GraphicsEnvironmentType::gpu_type& gpu) noexcept -> const GraphicsEnvironmentType::gpu_type&
		{
			return *(_gpu = &gpu);
		}

	public: // constructors
		/** @overload Constructs a freed [[window]].
		 * See [[freeable]] for more information.
		 * @see freeable
		 */
		window() = default;
		window(window&&) = default;
		auto operator=(window&&) -> window& = default;

	private:
		window(const GraphicsEnvironmentType::gpu_type* gpu, window_settings& settings) noexcept
		{
			if (settings.pixel_size.x() <= 0 || settings.pixel_size.y() <= 0)
				settings.pixel_size = int2({ 480, 480 });

			if (settings.name.empty())
				settings.name = "Window";

			_gpu = gpu;
			_pixel_size = settings.pixel_size;
		}
	public:
		/** Constructs a window on no specific gpu, with the given settings.
		 * Also sets invalid values of settings to some default value
		 */
		window(const GraphicsEnvironmentType& environment, window_settings& settings) noexcept
			: window(nullptr, settings) {}

		/** Constructs a window on the given gpu, with the given settings. */
		window(const GraphicsEnvironmentType::gpu_type& gpu, window_settings& settings) noexcept
			: window(&gpu, settings) {}

	public: // compwolf::freeable
		/** Invoked right before the window's data is freed.
		 * @see free()
		 */
		event<> freeing;
		/** Invoked right after the window's data is freed.
		 * @see free()
		 */
		event<> freed;

	protected:
		virtual void on_free() noexcept = 0;

	public:
		auto empty() const noexcept -> bool final
		{
			return !_gpu;
		}
		/** @see freeable */
		void free() noexcept final
		{
			if (empty()) return;

			freeing();
			on_free();
			_gpu = nullptr;
			freed();
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW
