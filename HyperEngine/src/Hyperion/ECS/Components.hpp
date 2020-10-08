#pragma once

#include <string>

#include "Maths/Matrix.hpp"
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

	struct CameraComponent
	{
		uint32_t Width;
		uint32_t Height;

		float Zoom;
		float NearPlane;
		float FarPlane;

		Matrix4<float> ProjectionMatrix = Matrix4<float>(1.0f);
		Matrix4<float> TransformationMatrix = Matrix4<float>(1.0f);

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(uint32_t width, uint32_t height, float zoom, float nearPlane, float farPlane)
			: Width(width), Height(height), Zoom(zoom), NearPlane(nearPlane), FarPlane(farPlane) {}
	};

	struct CameraControllerComponent
	{
		float Speed;

		CameraControllerComponent() = default;
		CameraControllerComponent(const CameraControllerComponent&) = default;
		CameraControllerComponent(float speed)
			: Speed(speed) {}
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
