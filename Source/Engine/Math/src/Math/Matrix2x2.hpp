#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class Matrix2x2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Matrix2x2!");
	}
	
	using Mat2x2 = Matrix2x2<float>;
	using Mat2x2f = Matrix2x2<float>;
	using Mat2x2d = Matrix2x2<double>;
	using Mat2x2i = Matrix2x2<int32_t>;
	using Mat2x2ui = Matrix2x2<uint32_t>;
}
