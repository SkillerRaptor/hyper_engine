#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class vector2
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Vector2!");
	
	public:
		vector2()
			: x{ static_cast<T>(0) }
			, y{ static_cast<T>(0) }
		{
		}
		
		vector2(T scalar)
			: x{ scalar }
			, y{ scalar }
		{
		}
		
		vector2(T x, T y)
			: x{ x }
			, y{ y }
		{
		}
	
	public:
		union
		{
			T data[2];
			
			struct
			{
				T x;
				T y;
			};
			
			struct
			{
				T s;
				T t;
			};
			
			struct
			{
				T u;
				T v;
			};
		};
	};
	
	using vec2 = vector2<float>;
	using vec2f = vector2<float>;
	using vec2d = vector2<double>;
	using vec2i = vector2<int>;
	using vec2ui = vector2<unsigned int>;
}
