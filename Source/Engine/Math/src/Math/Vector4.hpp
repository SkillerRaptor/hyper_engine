#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class Vector4
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Vector4!");
	
	public:
		Vector4()
			: x{ static_cast<T>(0) }
			, y{ static_cast<T>(0) }
			, z{ static_cast<T>(0) }
			, w{ static_cast<T>(0) }
		{
		}
		
		Vector4(T scalar)
			: x{ scalar }
			, y{ scalar }
			, z{ scalar }
			, w{ scalar }
		{
		}
		
		Vector4(T x, T y, T z, T w)
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
	
	using Vec4 = Vector4<float>;
	using Vec4f = Vector4<float>;
	using Vec4d = Vector4<double>;
	using Vec4i = Vector4<int32_t>;
	using Vec4ui = Vector4<uint32_t>;
}
