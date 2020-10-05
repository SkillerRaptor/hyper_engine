#pragma once

#include <string>

#include "Maths/Vector.hpp"

namespace Hyperion
{
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent& tagComponent) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		Vector3 Position;
		Vector3 Rotation;
		Vector3 Scale;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& transformComponent) = default;
		TransformComponent(const Vector3& position, const Vector3& rotation, const Vector3& scale)
			: Position(position), Rotation(rotation), Scale(scale) {}
	};

	struct SpriteRendererComponent
	{
		Vector4 Color;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& spriteRendererComponent) = default;
		SpriteRendererComponent(const Vector4& color)
			: Color(color) {}
	};

	struct CharacterControllerComponent 
	{
		float Speed;

		CharacterControllerComponent() = default;
		CharacterControllerComponent(const CharacterControllerComponent& characterControllerComponent) = default;
		CharacterControllerComponent(float speed)
			: Speed(speed) {}
	};
}