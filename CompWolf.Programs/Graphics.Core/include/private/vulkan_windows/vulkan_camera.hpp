#ifndef COMPWOLF_GRAPHICS_VULKAN_CAMERA
#define COMPWOLF_GRAPHICS_VULKAN_CAMERA

#include <windows>
#include <vulkan_programs>
#include "vulkan_window.hpp"
#include <vector>
#include <cstddef>

namespace compwolf::vulkan
{
	class vulkan_window;
	class swapchain_frame;

	/** Aggregate type containing data passed by [[vulkan_gpu_program]], which are created by [[vulkan_camera]] for drawing, to its code. */
	struct vulkan_draw_code_parameters : public vulkan_code_parameters
	{
		/** The window to draw onto.
		 * @see vulkan_window
		 */
		const vulkan_window* window;
		/** The window's frame to draw onto.
		 * @see swapchain_frame
		 */
		const swapchain_frame* frame;
		/** The index of the window's frame in [[window_swapchain::frames]]. */
		std::size_t frame_index;
	};

	/** Vulkan implementation of [[window_camera]].
	 * @see window_camera
	 */
	class vulkan_camera : public window_camera<vulkan_window>
	{
		std::vector<vulkan_gpu_program> _draw_programs;
		event<const vulkan_draw_code_parameters&> _drawing_code;
		event_key<> _drawing_key;

	public:
		/** The key used to identify some drawing-code added to a camera with [[vulkan_camera::add_draw_code]]. */
		using draw_code_key = event<const vulkan_draw_code_parameters&>::key_type;

	public: // modifiers
		/** Adds the given gpu code to be run when the window's camera is being updated.
		 * @return a key used to identify the piece of code.
		 */
		auto add_draw_code(std::function<void(const vulkan_draw_code_parameters&)> code) -> draw_code_key
		{
			_draw_programs.clear();
			return _drawing_code.subscribe(code);
		}
		/** Removes the given gpu code from being run when the window's camera is being updated. */
		void remove_draw_code(draw_code_key code) noexcept
		{
			_draw_programs.clear();
			return _drawing_code.unsubscribe(std::move(code));
		}

	protected:
		/** Sets this camera to a default-constructed camera. */
		void destruct() noexcept override
		{
			this->~vulkan_camera();
			new(this)vulkan_camera();
		}

	public: // constructors
		/** Constructs an invalid [[vulkan_camera]].
		 * Using this camera is undefined behaviour.
		 * @overload
		 */
		vulkan_camera() = default;
		vulkan_camera(vulkan_camera&&) = default;
		auto operator=(vulkan_camera&&)->vulkan_camera & = default;

		/** Constructs a camera for the given [[vulkan_window]], with the given settings. */
		vulkan_camera(vulkan_window& window, window_camera_settings settings);
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_CAMERA
