#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace Hyperion 
{
	namespace Vector
	{
		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		Vector2<T> Normalize(Vector2<T>& vector)
		{
			Vector2<T> vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			return vec;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		Vector3<T> Normalize(Vector3<T>& vector)
		{
			Vector3<T> vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			vec.z = vector.z / vector.Magnitude();
			return vec;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		Vector4<T> Normalize(Vector4<T>& vector)
		{
			Vector4<T> vec;
			vec.x = vector.x / vector.Magnitude();
			vec.y = vector.y / vector.Magnitude();
			vec.z = vector.z / vector.Magnitude();
			vec.w = vector.w / vector.Magnitude();
			return vec;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		float Dot(Vector2<T>& vectorOne, Vector2<T>& vectorTwo)
		{
			Vector2<T> vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		float Dot(Vector3<T>& vectorOne, Vector3<T>& vectorTwo)
		{
			Vector3<T> vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y + vec.z;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		float Dot(Vector4<T>& vectorOne, Vector4<T>& vectorTwo)
		{
			Vector4<T> vec = Normalize(vectorOne) * Normalize(vectorTwo);
			return vec.x + vec.y + vec.z + vec.w;
		}

		template <typename T = float, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		Vector3<T> Cross(Vector3<T>& vectorOne, Vector3<T>& vectorTwo)
		{
			Vector3<T> vec;
			vec.x = vectorOne.y * vectorTwo.z - vectorOne.z * vectorTwo.y;
			vec.y = vectorOne.z * vectorTwo.x - vectorOne.x * vectorTwo.z;
			vec.z = vectorOne.x * vectorTwo.y - vectorOne.y * vectorTwo.x;
			return vec;
		}
	};
}
