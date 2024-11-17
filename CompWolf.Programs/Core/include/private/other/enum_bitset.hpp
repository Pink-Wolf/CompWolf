#ifndef COMPWOLF_ENUM_BITSET_HEADER
#define COMPWOLF_ENUM_BITSET_HEADER
#include <utility>
#include <bitset>
#include <initializer_list>

namespace compwolf
{
    /** A set of an enum's elements.
     * Specifically an std::bitset whose bits represent different elements from an enum.
     * @typeparam EnumType The enum whose elements this set can contain.
     * An enum whose last element is "size", and whose elements do not have explicitly defined values, is a valid enum.
     * In general:
     * * The element's values must be explicitly convertible to std::size_t.
     * * If the compwolf::enum_bitset's second template parameter is not specified, the enum's last element must be "size".
     * @typeparam Size The highest value of the given enum.
     * More specifically, the highest value from the enum that the compwolf::enum_bitset can contain.
     * Passing an enum value higher than this is undefined behaviour.
     */
    template <typename EnumType,
        std::size_t Size = static_cast<std::size_t>(EnumType::size) + 1
    >
    struct enum_bitset : public std::bitset<Size>
    {
    private:
        using super = std::bitset<Size>;
    private:
        constexpr std::size_t to_pos(EnumType element) const noexcept
        {
            return static_cast<std::size_t>(element);
        }
    public: // accessors
        /** Whether the set contains the given element.
         * As in, returns the bit representing the given element.
         */
        constexpr auto test(EnumType element) const noexcept
            -> bool
        {
            return super::operator[](to_pos(element));
        }
        /** Whether the set contains the given element.
         * As in, returns the bit representing the given element.
         */
        constexpr auto operator[](EnumType element) const noexcept
            -> bool
        {
            return test(element);
        }
        /** Reference to whether the set contains the given element.
         * As in, returns the bit representing the given element.
         */
        constexpr auto operator[](EnumType element) noexcept
            -> super::reference
        {
            return super::operator[](to_pos(element));
        }
    public: // modifiers
        /** Sets whether the set contains the given element.
         * As in, sets the bit representing the given element to the given boolean value.
         * @return the set.
         */
        constexpr auto set(EnumType element, bool value = true) noexcept
            -> enum_bitset&
        {
            super::set(to_pos(element), value);
            return *this;
        }
        /** Removes the given element from the set.
         * As in, sets the bit representing the given element to 0.
         * @return the set.
         */
        constexpr auto reset(EnumType element) noexcept
            -> enum_bitset&
        {
            super::reset(to_pos(element));
            return *this;
        }
        /** If the set contains the given element, removes it;
         * Otherwise, adds the given element to the set.
         * As in, flips the bit representing the given element.
         * @return the set.
         */
        constexpr auto flip(EnumType element) noexcept
            -> enum_bitset&
        {
            super::flip(to_pos(element));
            return *this;
        }
    public: // constructors
        constexpr enum_bitset() noexcept = default;
        /** compwolf::enum_bitset can be constructed the same ways std::bitset can be constructed. */
        using super::bitset;
        constexpr enum_bitset(EnumType element) : super(1l << to_pos(element)) {}
        /** This ignores additional instances of the same element in std::initializer_list. */
        constexpr enum_bitset(std::initializer_list<EnumType> elements)
        {
            for (auto element : elements)
            {
                set(element);
            }
        }
        /** This ignores additional instances of the same element in std::initializer_list. */
        constexpr auto operator=(std::initializer_list<EnumType> elements)
            -> enum_bitset&
        {
            reset();
            for (auto element : elements)
            {
                set(element);
            }
            return *this;
        }
    };
}

#endif // ! COMPWOLF_ENUM_BITSET_HEADER
