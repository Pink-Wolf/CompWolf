#ifndef COMPWOLF_GRAPHICS_WINDOW_CAMERA
#define COMPWOLF_GRAPHICS_WINDOW_CAMERA

#include "window_settings.hpp"
#include <dimensions>
#include <events>

namespace compwolf
{
	struct window_camera_settings
	{
		/** How far towards the window's left border that the camera will be displayed.
		 * -1 is the window's right border, 1 is the window's left border.
		 */
		float screen_left = 1;
		/** How far towards the window's right border that the camera will be displayed.
		 * -1 is the window's left border, 1 is the window's right border.
		 */
		float screen_right = 1;
		/** How far towards the window's top border that the camera will be displayed.
		 * -1 is the window's bottom border, 1 is the window's top border.
		 */
		float screen_top = 1;
		/** How far towards the window's bottom border that the camera will be displayed.
		 * -1 is the window's bottom border, 1 is the window's top border.
		 */
		float screen_bottom = 1;

		/** The color the camera displays where there is nothing */
		float3 background_color;
	};

	/** A rectangular part of a window that you can actually draw onto.
	 * @typeparam WindowType The type of [[window]] that this image can be on.
	 */
	template <typename WindowType>
	class window_camera
	{
	public:
		/** The type of [[window]] that this image can be on. */
		using window_type = WindowType;

	private:
		window_type* _window{};

		float _left{}, _right{}, _top{}, _bottom{};

		/** The color the camera displays where there is nothing */
		float3 _background_color{};

		event_key<> _window_destructing_key{};

	public: // accessors
		/** The [[window]] that this image is on. */
		auto window() noexcept -> window_type& { return *_window; };
		/** The [[window]] that this image is on. */
		auto window() const noexcept -> const window_type& { return *_window; };
		/** Returns the gpu that the camera is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> typename window_type::environment_type::gpu_type& { return window().gpu(); }
		/** Returns the gpu that the camera is on. */
		auto gpu() const noexcept -> const typename window_type::environment_type::gpu_type& { return window().gpu(); }

		/** How far towards the window's left border that the camera will be displayed.
		 * -1 is the window's right border, 1 is the window's left border.
		 */
		auto screen_left() const noexcept -> float { return _left; };
		/** How far towards the window's right border that the camera will be displayed.
		 * -1 is the window's left border, 1 is the window's right border.
		 */
		auto screen_right() const noexcept -> float { return _right; };
		/** How far towards the window's top border that the camera will be displayed.
		 * -1 is the window's bottom border, 1 is the window's top border.
		 */
		auto screen_top() const noexcept -> float { return _top; };
		/** How far towards the window's bottom border that the camera will be displayed.
		 * -1 is the window's bottom border, 1 is the window's top border.
		 */
		auto screen_bottom() const noexcept -> float { return _bottom; };

		/** The color the camera displays where there is nothing */
		auto background_color() const noexcept -> float3 { return _background_color; }

	protected: // modifiers
		/** Sets this camera to a default-constructed camera. */
		virtual void destruct() noexcept
		{
			this->~window_camera();
			new(this)window_camera();
		}

	public: // constructors
		/** Constructs an invalid [[window_camera]].
		 * Using this image is undefined behaviour.
		 * @overload
		 */
		window_camera() = default;
		window_camera(window_camera&&) = default;
		auto operator=(window_camera&&) -> window_camera& = default;

		/** Constructs a camera for the given window, with the given settings. */
		window_camera(window_type& window, window_camera_settings settings) noexcept
			: _window(&window)
			, _left(settings.screen_left), _right(settings.screen_right)
			, _top(settings.screen_top), _bottom(settings.screen_bottom)
			, _background_color(settings.background_color)
			, _window_destructing_key(window.destructing().subscribe([this]()
				{
					destruct();
				}))
		{}
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_CAMERA
