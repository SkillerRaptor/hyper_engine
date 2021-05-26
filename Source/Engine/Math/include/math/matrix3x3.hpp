#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class matrix3x3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Matrix3x3!");
	}
	
	using mat3x3 = matrix3x3<float>;
	using mat3x3f = matrix3x3<float>;
	using mat3x3d = matrix3x3<double>;
	using mat3x3i = matrix3x3<int>;
	using mat3x3ui = matrix3x3<unsigned int>;
}
