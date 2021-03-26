#pragma once

#include <cstdint>
#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	class Vector2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Template argument T is not a valid scalar type of Vector2");
		
	public:
		T x;
		T y;
	};
	
	typedef Vector2<float> Vec2;
	typedef Vector2<double> Vec2d;
	typedef Vector2<int32_t> Vec2i;
	typedef Vector2<uint32_t> Vec2ui;
}