#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class matrix4x4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Matrix4x4!");
	}
	
	using mat4x4 = matrix4x4<float>;
	using mat4x4f = matrix4x4<float>;
	using mat4x4d = matrix4x4<double>;
	using mat4x4i = matrix4x4<int>;
	using mat4x4ui = matrix4x4<unsigned int>;
}
