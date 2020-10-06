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
		Vector3<float> Position;
		Vector3<float> Rotation;
		Vector3<float> Scale;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& transformComponent) = default;
		TransformComponent(const Vector3<float>& position, const Vector3<float>& rotation, const Vector3<float>& scale)
			: Position(position), Rotation(rotation), Scale(scale) {}
	};

	struct SpriteRendererComponent
	{
		Vector4<float> Color;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& spriteRendererComponent) = default;
		SpriteRendererComponent(const Vector4<float>& color)
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