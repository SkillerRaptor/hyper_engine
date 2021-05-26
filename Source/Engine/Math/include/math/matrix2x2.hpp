#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class matrix2x2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Matrix2x2!");
	}
	
	using mat2x2 = matrix2x2<float>;
	using mat2x2f = matrix2x2<float>;
	using mat2x2d = matrix2x2<double>;
	using mat2x2i = matrix2x2<int>;
	using mat2x2ui = matrix2x2<unsigned int>;
}
