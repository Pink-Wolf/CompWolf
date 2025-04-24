#ifndef COMPWOLF_GRAPHICS_GPU_COLOR
#define COMPWOLF_GRAPHICS_GPU_COLOR

#include <dimensions>
#include <cstdint>
#include <utility>
#include <initializer_list>
#include <algorithm>
#include "primitive_gpu_struct_info.hpp"

namespace compwolf
{
	class color_element
	{
	private:
		uint8_t _data;

	public: // modifiers
		/** Sets the color to the given value.
		 * @customoverload 0 is no color and 255 is the max amount of color.
		 */
		constexpr auto& set(uint8_t i) noexcept
		{
			_data = i; return *this;
		}
		/** Sets the color to the given value.
		 * 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 */
		constexpr auto& set(long double f) noexcept
		{
			_data = static_cast<uint8_t>(std::clamp(f * 255.L, 0.L, 255.L)); return *this;
		}
		/** Sets the color to the given value
		 * 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 * @customoverload
		 */
		constexpr auto& set(double f) noexcept
		{
			_data = static_cast<uint8_t>(std::clamp(f * 255., 0., 255.)); return *this;
		}
		/** Sets the color to the given value
		 * 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 * @customoverload
		 */
		constexpr auto& set(float f) noexcept
		{
			_data = static_cast<uint8_t>(std::clamp(f * 255.f, 0.f, 255.f)); return *this;
		}

	public: // operators
		/** [[color_element]] can be explicit converted to a long double, where no color is 0 and the max amount of color is 1. */
		constexpr explicit operator long double() const noexcept { return _data / 255.L; }
		/** [[color_element]] can be explicit converted to a double, where no color is 0 and the max amount of color is 1. */
		constexpr explicit operator double() const noexcept { return _data / 255.; }
		/** [[color_element]] can be explicit converted to a float, where no color is 0 and the max amount of color is 1. */
		constexpr explicit operator float() const noexcept { return _data / 255.f; }
		/** [[color_element]] can be explicit converted to a uint8_t, where no color is 0 and the max amount of color is 255. */
		constexpr explicit operator uint8_t() const noexcept{ return _data; }

	public: // constructors
		/** Constructs a [[color_element]] with no color. */
		color_element() = default;
		color_element(const color_element&) = default;
		auto operator=(const color_element&) -> color_element& = default;
		color_element(color_element&&) = default;
		auto operator=(color_element&&) -> color_element& = default;
		
		/** Constructs a [[color_element]] with the given value.
		 * @customoverload 0 is no color and 255 is the max amount of color.
		 */
		explicit constexpr color_element(uint8_t i) noexcept : _data(i) {}
		/** Constructs a [[color_element]] with the given value.
		 * 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 */
		explicit constexpr color_element(long double f) noexcept { set(f); }
		/** Constructs a [[color_element]] with the given value.
		 * 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 * @customoverload
		 */
		explicit constexpr color_element(double f) noexcept { set(f); }
		/** Constructs a [[color_element]] with the given value.
		 * 0 is no color and 1 is the max amount of color.
		 * Values below 0 or above 1 are clamped to 0/1 respectively.
		 * @customoverload
		 */
		explicit constexpr color_element(float f) noexcept { set(f); }
	};

	constexpr auto operator+(color_element lhs, color_element rhs) noexcept { return color_element(static_cast<uint8_t>(static_cast<uint8_t>(lhs) + static_cast<uint8_t>(rhs))); }
	constexpr auto operator-(color_element lhs, color_element rhs) noexcept { return color_element(static_cast<uint8_t>(static_cast<uint8_t>(lhs) - static_cast<uint8_t>(rhs))); }
	constexpr auto operator*(color_element lhs, color_element rhs) noexcept { return color_element(static_cast<uint8_t>(static_cast<uint8_t>(lhs) * static_cast<uint8_t>(rhs))); }
	constexpr auto operator/(color_element lhs, color_element rhs) noexcept { return color_element(static_cast<uint8_t>(static_cast<uint8_t>(lhs) / static_cast<uint8_t>(rhs))); }
	constexpr auto operator<=>(color_element lhs, color_element rhs) noexcept { return static_cast<uint8_t>(lhs) <=> static_cast<uint8_t>(rhs); }
	constexpr auto operator==(color_element lhs, color_element rhs) noexcept { return static_cast<uint8_t>(lhs) == static_cast<uint8_t>(rhs); }
	constexpr auto operator!=(color_element lhs, color_element rhs) noexcept { return static_cast<uint8_t>(lhs) != static_cast<uint8_t>(rhs); }

	class transparent_color;
	/** A color made out of a mix of red, green, and blue. */
	class opaque_color : public dimensional_array<color_element, 3>
	{
		/** Returns the amount of red used to make the color. */
		constexpr auto& r() noexcept { return x(); }
		/** Returns the amount of red used to make the color. */
		constexpr auto& r() const noexcept { return x(); }
		/** Returns the amount of green used to make the color. */
		constexpr auto& g() noexcept { return y(); }
		/** Returns the amount of green used to make the color. */
		constexpr auto& g() const noexcept { return y(); }
		/** Returns the amount of blue used to make the color. */
		constexpr auto& b() noexcept { return z(); }
		/** Returns the amount of blue used to make the color. */
		constexpr auto& b() const noexcept { return z(); }

		/** A black color. */
		opaque_color() = default;
		opaque_color(const opaque_color&) = default;
		auto operator=(const opaque_color&) -> opaque_color& = default;
		opaque_color(opaque_color&&) = default;
		auto operator=(opaque_color&&) -> opaque_color& = default;

		/** Constructs a color from mixing the given amount of red, green, and blue. */
		constexpr opaque_color(color_element r, color_element g, color_element b) noexcept
			: dimensional_array<color_element, 3>({ r, g, b }) {}
		/** Constructs a color from mixing the given amount of red, green, and blue.
		 * The values in initializer_list are red, green, and blue respectively.
		 */
		constexpr opaque_color(std::initializer_list<color_element> v) noexcept
		{
			auto i = v.begin();
			if (i) { r() = *i; ++i; }
			if (i) { g() = *i; ++i; }
			if (i) { b() = *i; ++i; }
		}
		/** Constructs a color from mixing the given amount of red, green, and blue,
		 * The values in initializer_list are red, green, and blue respectively.
		 * The floats are converted to single_colors.
		 */
		constexpr opaque_color(std::initializer_list<float> v) noexcept
		{
			auto i = v.begin();
			if (i) { r() = single_color(*i); ++i; }
			if (i) { g() = single_color(*i); ++i; }
			if (i) { b() = single_color(*i); ++i; }
		}

		constexpr operator transparent_color() const noexcept;
	};
	/** A color made out of a mix of red, green, blue, and alpha. */
	class transparent_color : public dimensional_array<color_element, 4>
	{
		/** Returns the amount of red used to make the color. */
		constexpr auto& r() noexcept { return x(); }
		/** Returns the amount of red used to make the color. */
		constexpr auto& r() const noexcept { return x(); }
		/** Returns the amount of green used to make the color. */
		constexpr auto& g() noexcept { return y(); }
		/** Returns the amount of green used to make the color. */
		constexpr auto& g() const noexcept { return y(); }
		/** Returns the amount of blue used to make the color. */
		constexpr auto& b() noexcept { return z(); }
		/** Returns the amount of blue used to make the color. */
		constexpr auto& b() const noexcept { return z(); }
		/** Returns the amount of alpha used to make the color. */
		constexpr auto& a() noexcept { return w(); }
		/** Returns the amount of alpha used to make the color. */
		constexpr auto& a() const noexcept { return w(); }

		/** A black color. */
		transparent_color() = default;
		transparent_color(const transparent_color&) = default;
		auto operator=(const transparent_color&) -> transparent_color& = default;
		opaque_color(transparent_color&&) = default;
		auto operator=(transparent_color&&) -> transparent_color& = default;

		/** Constructs a color from mixing the given amount of red, green, and blue, and using the given alpha. */
		constexpr transparent_color(color_element r, color_element g, color_element b, color_element a) noexcept
			: dimensional_array<color_element, 4>({ r, g, b, a }) {}
		/** Constructs a color from mixing the given amount of red, green, and blue, and using the given alpha.
		 * The values in initializer_list are red, green, blue, and alpha respectively.
		 */
		constexpr transparent_color(std::initializer_list<color_element> v) noexcept
		{
			auto i = v.begin();
			if (i) { r() = *i; ++i; }
			if (i) { g() = *i; ++i; }
			if (i) { b() = *i; ++i; }
			if (i) { a() = *i; ++i; }
		}
		/** Constructs a color from mixing the given amount of red, green, and blue, and using the given alpha.
		 * The values in initializer_list are red, green, blue, and alpha respectively.
		 * The floats are converted to single_colors.
		 */
		constexpr transparent_color(std::initializer_list<float> v) noexcept
		{
			auto i = v.begin();
			if (i) { r() = single_color(*i); ++i; }
			if (i) { g() = single_color(*i); ++i; }
			if (i) { b() = single_color(*i); ++i; }
			if (i) { a() = *i; ++i; }
		}

		constexpr operator opaque_color() const noexcept;
	};

	/** @hidden */
	constexpr opaque_color::operator transparent_color() const noexcept { return transparent_color{ r(), g(), b(), color_element(1.f) }; }
	/** @hidden */
	constexpr transparent_color::operator opaque_color() const noexcept { return opaque_color{ r(), g(), b() }; }

	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(opaque_color);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(transparent_color);
}

#endif // ! COMPWOLF_GRAPHICS_GPU_COLOR
