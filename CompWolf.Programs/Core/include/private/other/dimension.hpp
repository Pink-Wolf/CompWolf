#ifndef COMPWOLF_DIMENSIONS
#define COMPWOLF_DIMENSIONS

#include <array>
#include <initializer_list>
#include <cstddef>
#include <concepts>
#include <stdexcept>

namespace compwolf
{
	/** A math-vector, matrix, or higher dimension array of numbers.
	 * That is, a 1- or multidimensional array, where the amount of values in template argument SizeInDimensions is the amount of dimensions of the array.
	 *
	 * @typeparam ValueType The type of value contained by the dimension.
	 *
	 * @typeparam SizeInDimensions If this contains a single number, then dimensional_array is a math-vector/array, with the given size.
	 * If this contains 2 numbers, then dimensional_array is a matrix/2d array, with the given amount of rows and column.
	 * If this contains x numbers, then the pattern follows: dimensional_array is an x-dimensional array, with the given amount of elements in each dimension.
	 * All values must be positive.
	 */
	template <typename ValueType, std::size_t... SizeInDimensions>
		requires ((SizeInDimensions > 0) && ...)
	struct dimensional_array : public std::array<ValueType, (SizeInDimensions * ...)>
	{
	private:
		using base_array = std::array<ValueType, (SizeInDimensions * ...)>;

	private:
		static constexpr std::size_t _size_in_dimension[]{ SizeInDimensions... };

	public: // accessors
		/** Gets the amount of elements the dimensional_array contains in the given dimension.
		 * @throws std::domain_error if the given value is not within [0; amount of dimensions[.
		 */
		static constexpr auto size_in_dimension(std::size_t i)
		{
			if (i > sizeof...(SizeInDimensions)) throw std::out_of_range("dimensions::size_in_dimension was given a value not within [0; amount of dimensions[.");
			return _size_in_dimension[i];
		}

		/** If the dimension is a vector, gets the element at the given index.
		 * If the dimension is a matrix, gets the element at the given column and row.
		 * This pattern follows for higher-dimensional array.
		 *
		 * If this is given fewer indices than the dimensional_array has dimensions, for example at(1) for a matrix, then the remaining indices are assumed to be 0.
		 *
		 * @throws std::out_of_range if the given indices points to an element outside of the dimensional_array.
		 */
		constexpr auto at(std::initializer_list<std::size_t> indices) -> base_array::reference
		{
			std::size_t index_sum = 0;
			std::size_t index_count = 0;
			for (auto i = indices.begin(); i != indices.end(); ++i, ++index_count)
			{
				auto& index = *i;
				index_sum += index * ((index_count == 0) ? 1 : size_in_dimension(index_count - 1));
			}
			return base_array::at(index_sum);
		}
		/** @overload If the dimension is a vector, gets the element at the given index.
		 * If the dimension is a matrix, gets the element at the given column and row.
		 * This pattern follows for higher-dimensional array.
		 *
		 * If this is given fewer indices than the dimensional_array has dimensions, for example at(1) for a matrix, then the remaining indices are assumed to be 0.
		 *
		 * @throws std::out_of_range if the given indices points to an element outside of the dimensional_array.
		 */
		constexpr auto at(std::initializer_list<std::size_t> indices) const -> base_array::const_reference
		{
			return const_cast<dimensional_array*>(this)->at(indices);
		}

	private:
		static consteval auto has_element_at(std::initializer_list<std::size_t> indices) noexcept -> bool
		{
			std::size_t index_count = 0;
			for (auto i = indices.begin(); i != indices.end(); ++i, ++index_count)
			{
				auto index = *i;
				if (index_count >= sizeof...(SizeInDimensions)) return false;
				if (index >= size_in_dimension(index_count)) return false;
			}
			return true;
		}
	public:
		/** @hidden */
#define COMPWOLF_DIMENSIONS_DEFINE_GETTER(getter, index)		\
		constexpr auto& getter() noexcept						\
			requires (has_element_at index)						\
		{														\
			return at index;									\
		}														\
																\
		constexpr auto& getter() const noexcept					\
			requires (has_element_at index)						\
		{														\
			return at index;									\
		}														\

		/** Returns at(0).
		 * @customcode constexpr auto& x() noexcept requires (has_element_at(0)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(x, ({ 0 }));
		/** Returns at(1).
		 * @customcode constexpr auto& y() noexcept requires (has_element_at(1)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(y, ({ 1 }));
		/** Returns at(2).
		 * @customcode constexpr auto& z() noexcept requires (has_element_at(2)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(z, ({ 2 }));
		/** Returns at(3).
		 * @customcode constexpr auto& w() noexcept requires (has_element_at(3)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(w, ({ 3 }));

		/** Returns at(0, 0).
		 * @customcode constexpr auto& xx() noexcept requires (has_element_at(0, 0)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(xx, ({ 0, 0 }));
		/** Returns at(0, 1).
		 * @customcode constexpr auto& xy() noexcept requires (has_element_at(0, 1)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(xy, ({ 0, 1 }));
		/** Returns at(0, 2).
		 * @customcode constexpr auto& xz() noexcept requires (has_element_at(0, 2)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(xz, ({ 0, 2 }));
		/** Returns at(0, 3).
		 * @customcode constexpr auto& xw() noexcept requires (has_element_at(0, 3)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(xw, ({ 0, 3 }));

		/** Returns at(1, 0).
		 * @customcode constexpr auto& yx() noexcept requires (has_element_at(1, 0)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(yx, ({ 1, 0 }));
		/** Returns at(1, 1).
		 * @customcode constexpr auto& yy() noexcept requires (has_element_at(1, 1)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(yy, ({ 1, 1 }));
		/** Returns at(1, 2).
		 * @customcode constexpr auto& yz() noexcept requires (has_element_at(1, 2)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(yz, ({ 1, 2 }));
		/** Returns at(1, 3).
		 * @customcode constexpr auto& yw() noexcept requires (has_element_at(1, 3)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(yw, ({ 1, 3 }));

		/** Returns at(2, 0).
		 * @customcode constexpr auto& zx() noexcept requires (has_element_at(2, 0)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(zx, ({ 2, 0 }));
		/** Returns at(2, 1).
		 * @customcode constexpr auto& zy() noexcept requires (has_element_at(2, 1)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(zy, ({ 2, 1 }));
		/** Returns at(2, 2).
		 * @customcode constexpr auto& zz() noexcept requires (has_element_at(2, 2)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(zz, ({ 2, 2 }));
		/** Returns at(2, 3).
		 * @customcode constexpr auto& zw() noexcept requires (has_element_at(2, 3)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(zw, ({ 2, 3 }));

		/** Returns at(3, 0).
		 * @customcode constexpr auto& wx() noexcept requires (has_element_at(3, 0)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(wx, ({ 3, 0 }));
		/** Returns at(3, 1).
		 * @customcode constexpr auto& wy() noexcept requires (has_element_at(3, 1)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(wy, ({ 3, 1 }));
		/** Returns at(3, 2).
		 * @customcode constexpr auto& wz() noexcept requires (has_element_at(3, 2)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(wz, ({ 3, 2 }));
		/** Returns at(3, 3).
		 * @customcode constexpr auto& ww() noexcept requires (has_element_at(3, 3)) {};
		 */
		COMPWOLF_DIMENSIONS_DEFINE_GETTER(ww, ({ 3, 3 }));
	};

#define COMPWOLF_DEFINE_DIMENSIONAL_ARRAY_TYPES(type)	\
	using type##2 = dimensional_array<type, 2>;			\
	using type##3 = dimensional_array<type, 3>;			\
	using type##4 = dimensional_array<type, 4>;			\
	using type##2x2 = dimensional_array<type, 2, 2>;	\
	using type##2x3 = dimensional_array<type, 2, 3>;	\
	using type##2x4 = dimensional_array<type, 2, 4>;	\
	using type##3x2 = dimensional_array<type, 3, 2>;	\
	using type##3x3 = dimensional_array<type, 3, 3>;	\
	using type##3x4 = dimensional_array<type, 3, 4>;	\
	using type##4x2 = dimensional_array<type, 4, 2>;	\
	using type##4x3 = dimensional_array<type, 4, 3>;	\
	using type##4x4 = dimensional_array<type, 4, 4>		\

	/** @hidden */
	COMPWOLF_DEFINE_DIMENSIONAL_ARRAY_TYPES(float);
	/** @hidden */
	COMPWOLF_DEFINE_DIMENSIONAL_ARRAY_TYPES(double);
	/** @hidden */
	COMPWOLF_DEFINE_DIMENSIONAL_ARRAY_TYPES(int);
	/** @hidden */
	COMPWOLF_DEFINE_DIMENSIONAL_ARRAY_TYPES(bool);
}

#endif // ! COMPWOLF_DIMENSIONS
