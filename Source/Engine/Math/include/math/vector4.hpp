#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class vector4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Vector4!");
	
	public:
		vector4()
			: x{ static_cast<T>(0) }
			, y{ static_cast<T>(0) }
			, z{ static_cast<T>(0) }
			, w{ static_cast<T>(0) }
		{
		}
		
		vector4(T scalar)
			: x{ scalar }
			, y{ scalar }
			, z{ scalar }
			, w{ scalar }
		{
		}
		
		vector4(T x, T y, T z, T w)
			: x{ x }
			, y{ y }
			, z{ z }
			, w{ w }
		{
		}
	
	public:
		union
		{
			T data[4];
			
			struct
			{
				T x;
				T y;
				T z;
				T w;
			};
			
			struct
			{
				T r;
				T g;
				T b;
				T a;
			};
			
			struct
			{
				T s;
				T t;
				T p;
				T q;
			};
		};
	};
	
	using vec4 = vector4<float>;
	using vec4f = vector4<float>;
	using vec4d = vector4<double>;
	using vec4i = vector4<int>;
	using vec4ui = vector4<unsigned int>;
}
