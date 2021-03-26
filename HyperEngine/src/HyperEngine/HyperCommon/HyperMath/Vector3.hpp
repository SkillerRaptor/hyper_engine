#pragma once

#include <cstdint>
#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	class Vector3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Template argument T is not a valid scalar type of Vector3");
	
	public:
		T x;
		T y;
		T z;
	};
	
	typedef Vector3<float> Vec3;
	typedef Vector3<double> Vec3d;
	typedef Vector3<int32_t> Vec3i;
	typedef Vector3<uint32_t> Vec3ui;
}