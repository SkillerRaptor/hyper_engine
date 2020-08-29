#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Hyperion 
{
	class Vector
	{
	public:
		static Vector2 normalize(Vector2& vector);
		static Vector3 normalize(Vector3& vector);
		static Vector4 normalize(Vector4& vector);

		static float dot(Vector2& vectorOne, Vector2& vectorTwo);
		static float dot(Vector3& vectorOne, Vector3& vectorTwo);
		static float dot(Vector4& vectorOne, Vector4& vectorTwo);

		static Vector3 cross(Vector3& vectorOne, Vector3& vectorTwo);
	};
}