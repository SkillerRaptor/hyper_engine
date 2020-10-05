#pragma once

#include "Maths/Vector.hpp"

namespace Hyperion
{
	struct TransformComponent
	{
		Vector3 Position;
		Vector3 Rotation;
		Vector3 Scale;

		TransformComponent(Vector3 position = { 0.0f, 0.0f, 0.0f }, Vector3 rotation = { 0.0f, 0.0f, 0.0f }, Vector3 scale = { 0.0f, 0.0f, 0.0f })
			: Position(position), Rotation(rotation), Scale(scale) {}
	};

	struct CharacterControllerComponent 
	{
		float Speed;

		CharacterControllerComponent(float speed)
			: Speed(speed) {}
	};
}