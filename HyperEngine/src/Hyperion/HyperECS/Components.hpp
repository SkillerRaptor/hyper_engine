#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

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
		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& transformComponent) = default;
		TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
			: Position(position), Rotation(rotation), Scale(scale) {}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& spriteRendererComponent) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		uint32_t Width;
		uint32_t Height;

		float Zoom;
		float NearPlane;
		float FarPlane;

		bool Primary;
		enum class CameraTypeInfo
		{
			ORTHOGRAPHIC,
			PROJECTION
		} CameraType;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(uint32_t width, uint32_t height, float zoom, float nearPlane, float farPlane, bool primary, CameraTypeInfo cameraType)
			: Width(width), Height(height), Zoom(zoom), NearPlane(nearPlane), FarPlane(farPlane), Primary(primary), CameraType(cameraType) {}
	};

	struct CameraControllerComponent
	{
		float MoveSpeed;
		float ZoomSpeed;

		CameraControllerComponent() = default;
		CameraControllerComponent(const CameraControllerComponent&) = default;
		CameraControllerComponent(float moveSpeed, float zoomSpeed)
			: MoveSpeed(moveSpeed), ZoomSpeed(zoomSpeed)  {}
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
