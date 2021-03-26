#pragma once

#include <cstdint>
#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	class Vector4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Template argument T is not a valid scalar type of Vector4");
	
	public:
		T x;
		T y;
		T z;
		T w;
	};
	
	typedef Vector4<float> Vec4;
	typedef Vector4<double> Vec4d;
	typedef Vector4<int32_t> Vec4i;
	typedef Vector4<uint32_t> Vec4ui;
}