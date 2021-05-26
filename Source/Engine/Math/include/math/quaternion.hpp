#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class quaternion
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Quaternion!");
	}
	
	using quat = quaternion<float>;
	using quatf = quaternion<float>;
	using quatd = quaternion<double>;
	using quati = quaternion<int>;
	using quatui = quaternion<unsigned int>;
}
