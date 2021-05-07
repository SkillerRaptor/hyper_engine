#pragma once

#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	class Matrix3x3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "HyperMath: Invalid arithmetic type for Matrix3x3!");
	}
	
	using Mat3x3 = Matrix3x3<float>;
	using Mat3x3f = Matrix3x3<float>;
	using Mat3x3d = Matrix3x3<double>;
	using Mat3x3i = Matrix3x3<int>;
	using Mat3x3ui = Matrix3x3<unsigned int>;
}
