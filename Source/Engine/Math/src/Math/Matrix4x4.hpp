#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class Matrix4x4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Matrix4x4!");
	}
	
	using Mat4x4 = Matrix4x4<float>;
	using Mat4x4f = Matrix4x4<float>;
	using Mat4x4d = Matrix4x4<double>;
	using Mat4x4i = Matrix4x4<int32_t>;
	using Mat4x4ui = Matrix4x4<uint32_t>;
}
