#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class Matrix3x3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Matrix3x3!");
	}
	
	using Mat3x3 = Matrix3x3<float>;
	using Mat3x3f = Matrix3x3<float>;
	using Mat3x3d = Matrix3x3<double>;
	using Mat3x3i = Matrix3x3<int32_t>;
	using Mat3x3ui = Matrix3x3<uint32_t>;
}
