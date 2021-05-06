#pragma once

#include <type_traits>

namespace HyperMath
{
	template <typename T>
	class Quaternion
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "HyperMath: Invalid arithmetic type for Quaternion!");
	}
	
	using Quat = Quaternion<float>;
	using Quatf = Quaternion<float>;
	using Quatd = Quaternion<double>;
	using Quati = Quaternion<int>;
	using Quatui = Quaternion<unsigned int>;
}
