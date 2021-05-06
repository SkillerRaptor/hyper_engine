#pragma once

#include <type_traits>

namespace HyperMath
{
	template <typename T>
	class Vector3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "HyperMath: Invalid arithmetic type for Vector3!");
	
	public:
		Vector3()
			: x{ static_cast<T>(0) }
			, y{ static_cast<T>(0) }
			, z{ static_cast<T>(0) }
		{
		}
	
	public:
		T x;
		T y;
		T z;
	};
	
	using Vec3 = Vector3<float>;
	using Vec3f = Vector3<float>;
	using Vec3d = Vector3<double>;
	using Vec3i = Vector3<int>;
	using Vec3ui = Vector3<unsigned int>;
}