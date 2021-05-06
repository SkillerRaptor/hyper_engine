#pragma once

#include <type_traits>

namespace HyperMath
{
	template <typename T>
	class Vector2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "HyperMath: Invalid arithmetic type for Vector2!");
	
	public:
		Vector2()
			: x{ static_cast<T>(0) }
			, y{ static_cast<T>(0) }
		{
		}
	
	public:
		T x;
		T y;
	};
	
	using Vec2 = Vector2<float>;
	using Vec2f = Vector2<float>;
	using Vec2d = Vector2<double>;
	using Vec2i = Vector2<int>;
	using Vec2ui = Vector2<unsigned int>;
}