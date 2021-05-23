#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class Quaternion
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Quaternion!");
	}
	
	using Quat = Quaternion<float>;
	using Quatf = Quaternion<float>;
	using Quatd = Quaternion<double>;
	using Quati = Quaternion<int>;
	using Quatui = Quaternion<unsigned int>;
}
