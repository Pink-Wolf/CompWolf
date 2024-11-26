#ifndef COMPWOLF_GRAPHICS_INPUT_STATE
#define COMPWOLF_GRAPHICS_INPUT_STATE

#include <map>
#include <events>
#include <set>
#include <cctype>
#include <stdexcept>

namespace compwolf
{
	/** Should only be constructed by [[input_state]].
	 * Contains an [[input_state]]'s data for a single key.
	 */
	class input_key_state
	{
		bool _down = false;
		bool _was_down = false;

		char _lower_character = 0;
		char _upper_character = 0;
		bool _is_uppercase = false;
	public:
		/** Returns whether this key is currently being pressed down. */
		constexpr auto down() const noexcept -> bool { return _down; }
		/** Returns whether this key is currently being pressed down, but was not last frame. */
		constexpr auto newly_down() const noexcept -> bool { return _down && !_was_down; }
		/** Returns whether this key is currently being pressed down, and was last frame as well. */
		constexpr auto continually_down() const noexcept -> bool { return _down && _was_down; }
		/** Returns whether this key was being pressed down last frame. */
		constexpr auto was_down() const noexcept -> bool { return _was_down; }

		/** Returns whether this key is currently not being pressed down. */
		constexpr auto up() const noexcept -> bool { return !_down; }
		/** Returns whether this key is currently not being pressed down, but was last frame. */
		constexpr auto newly_up() const noexcept -> bool { return !_down && _was_down; }
		/** Returns whether this key is currently not being pressed down, and was not last frame as well. */
		constexpr auto continually_up() const noexcept -> bool { return !_down && !_was_down; }
		/** Returns whether this key was not being pressed down last frame. */
		constexpr auto was_up() const noexcept -> bool { return !_was_down; }

		/** Returns what character the key represents, in lowercase. */
		auto lowercase_character() const noexcept -> char { return _lower_character; }
		/** Returns what character the key represents, capitalized. */
		auto uppercase_character() const noexcept -> char { return _upper_character; }
		/** Returns what character the key represents, or 0 (not '0') if it represents no key. */
		auto character() const noexcept -> char { return _is_uppercase ? _upper_character : _lower_character; }

		/** Returns whether the character should currently be interpreted as uppercase, instead of lowercase. */
		auto is_uppercase() const noexcept -> bool { return _is_uppercase; }
		/** Returns whether the character should currently be interpreted as uppercase, instead of lowercase. */
		auto is_lowercase() const noexcept -> bool { return !_is_uppercase; }

	public: // modifiers
		/** Sets whether the key is currently being pressed down. */
		void set_down(bool state = true) noexcept { _down = state; }
		/** Sets whether the key was being pressed down last frame. */
		void set_was_down(bool state = true) noexcept { _was_down = state; }
		/** Sets whether the character should currently be interpreted as uppercase, instead of lowercase. */
		void set_uppercase(bool state = true) noexcept { _is_uppercase = state; }

	public: // constructor
		/** Constructs a key that is not, and has not, been pressed down, and which does not represent a character.
		 * @overload
		 */
		constexpr input_key_state() noexcept = default;
		constexpr input_key_state(char c) noexcept : _lower_character(c), _upper_character(c + ('A' - 'a')) {}
	};

	/** Contains the inputs that a [[graphics_environment]] is currently receiving. */
	class input_state
	{
		mutable std::map<char, input_key_state> _chars;

		mutable std::map<char, event<const input_key_state&>> _char_newly_down;
		event<const input_key_state&> _any_char_newly_down;

		mutable std::map<char, event<const input_key_state&>> _char_newly_up;
		event<const input_key_state&> _any_char_newly_up;

	private:
		/** This treats uppercase and lowercase as the same character. */
		static auto sanatize_char(char c) -> char
			{ return std::tolower(c); }

	public: // accessors
		/** Gets the state for the key representing the given character.
		 * This treats uppercase and lowercase as the same character.
		 */
		auto state_for(char c) const noexcept -> const input_key_state&
		{
			c = sanatize_char(c);
			_chars.emplace(c, c);
			return _chars[c];
		}

		/** Gets an event for when the key representing the given character is beginning to be held down.
		 * This treats uppercase and lowercase as the same character.
		 */
		auto char_newly_down(char c) const noexcept -> const event<const input_key_state&>&
		{
			c = sanatize_char(c);
			_char_newly_down.emplace(c, event<const input_key_state&>());
			return _char_newly_down[c];
		}
		/** Gets an event for when any key representing a character is beginning to be held down. */
		auto char_newly_down() const noexcept -> const event<const input_key_state&>& { return _any_char_newly_down; }
		/** Gets an event for when the key representing the given character is being released from being held down.
		 * This treats uppercase and lowercase as the same character.
		 */
		auto char_newly_up(char c) const noexcept -> const event<const input_key_state&>&
		{
			c = sanatize_char(c);
			_char_newly_up.emplace(c, event<const input_key_state&>());
			return _char_newly_up[c];
		}
		/** Gets an event for when any key representing a character is being released from being held down. */
		auto char_newly_up() const noexcept -> const event<const input_key_state&>& { return _any_char_newly_up; }

	public: // modifiers
		/** Makes all of the state's keys' last-frame-data be equal to the current-frame-data. */
		void update_last_frame_data() noexcept
		{
			for (auto& c : _chars)
			{
				c.second.set_down(c.second.down());
			}
		}

		/** Tells the state whether the key representing the given character is up or down.
		 * Note that you must call [[input_state::update_last_frame_data]] first to update last-frame-data.
		 * This treats uppercase and lowercase as the same character.
		 */
		void set_char_state(char c, bool is_down, bool is_capitalized) noexcept
		{
			c = sanatize_char(c);
			auto stateI = _chars.find(c);
			if (stateI == _chars.end())
			{
				if (!is_down && !is_capitalized) return; // no state object = default state
				_chars.emplace(c, c);
				stateI = _chars.find(c);
			}

			auto& state = stateI->second;
			state.set_down(is_down);
			state.set_uppercase(is_capitalized);

			if (state.was_down() != is_down)
			{
				auto& char_event = is_down ? _char_newly_down : _char_newly_up;
				auto& any_event = is_down ? _any_char_newly_down : _any_char_newly_up;

				auto i = char_event.find(c);
				if (i != char_event.end()) i->second.invoke(state);
				any_event.invoke(state);
			}
		}

	public: // constructor
		input_state() = default;
		input_state(input_state&&) = default;
		auto operator=(input_state&&) -> input_state& = default;
	};
}

#endif // ! COMPWOLF_GRAPHICS_INPUT_STATE
