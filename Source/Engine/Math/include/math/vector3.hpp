#pragma once

#include <type_traits>

namespace Math
{
	template <typename T>
	class vector3
	{
	public:
		static_assert(std::is_arithmetic_v<T>, "Invalid template type for Vector3!");
	
	public:
		vector3()
			: x{ static_cast<T>(0) }
			, y{ static_cast<T>(0) }
			, z{ static_cast<T>(0) }
		{
		}
		
		vector3(T scalar)
			: x{ scalar }
			, y{ scalar }
			, z{ scalar }
		{
		}
		
		vector3(T x, T y, T z)
			: x{ x }
			, y{ y }
			, z{ z }
		{
		}
	
	public:
		union
		{
			T data[3];
			
			struct
			{
				T x;
				T y;
				T z;
			};
			
			struct
			{
				T r;
				T g;
				T b;
			};
			
			struct
			{
				T s;
				T t;
				T p;
			};
		};
	};
	
	using vec3 = vector3<float>;
	using vec3f = vector3<float>;
	using vec3d = vector3<double>;
	using vec3i = vector3<int>;
	using vec3ui = vector3<unsigned int>;
}
