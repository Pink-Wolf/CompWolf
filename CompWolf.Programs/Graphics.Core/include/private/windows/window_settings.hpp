#ifndef COMPWOLF_GRAPHICS_WINDOW_SETTINGS
#define COMPWOLF_GRAPHICS_WINDOW_SETTINGS

#include <string_view>
#include <dimensions>

namespace compwolf
{
	/* Aggregate type used by window's constructor to specify its behaviour. */
	struct window_settings
	{
		/* The name of the window.
		 * If the window has a border, then this may for example be displayed on the border.
		 */
		std::string_view name;
		/* The initial width and height of the window, in pixels.
		 * This size does not include any border around the window.
		 */
		int2 pixel_size;
	};
}

#endif // ! COMPWOLF_GRAPHICS_WINDOW_SETTINGS
