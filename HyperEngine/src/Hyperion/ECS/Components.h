#pragma once

#include "Component.h"
#include "Maths/Vector.h"

namespace Hyperion
{
	struct TransformComponent : public Component
	{
		Vector3 Position;
		Vector3 Rotation;
		Vector3 Scale;

		TransformComponent(Vector3 position = { 0.0f, 0.0f, 0.0f }, Vector3 rotation = { 0.0f, 0.0f, 0.0f }, Vector3 scale = { 0.0f, 0.0f, 0.0f })
			: Position(position), Rotation(rotation), Scale(scale) {}
	};

	struct CharacterControllerComponent : public Component
	{
		float Speed;

		CharacterControllerComponent(float speed)
			: Speed(speed) {}
	};
}