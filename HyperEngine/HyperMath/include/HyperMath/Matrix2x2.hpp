#pragma once

#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	class Matrix2x2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "HyperMath: Invalid arithmetic type for Matrix2x2!");
	}
	
	using Mat2x2 = Matrix2x2<float>;
	using Mat2x2f = Matrix2x2<float>;
	using Mat2x2d = Matrix2x2<double>;
	using Mat2x2i = Matrix2x2<int>;
	using Mat2x2ui = Matrix2x2<unsigned int>;
}
