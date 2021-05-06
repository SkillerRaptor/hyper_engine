#pragma once

#include <type_traits>

namespace HyperMath
{
	template <typename T>
	class Vector4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "HyperMath: Invalid arithmetic type for Vector4!");
	
	public:
		Vector4()
			: x{ static_cast<T>(0) }
			, y{ static_cast<T>(0) }
			, z{ static_cast<T>(0) }
			, w{ static_cast<T>(0) }
		{
		}
	
	public:
		T x;
		T y;
		T z;
		T w;
	};
	
	using Vec4 = Vector4<float>;
	using Vec4f = Vector4<float>;
	using Vec4d = Vector4<double>;
	using Vec4i = Vector4<int>;
	using Vec4ui = Vector4<unsigned int>;
}