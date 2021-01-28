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
		glm::vec4 BackgroundColor;

		enum class ProjectionType
		{
			ORTHOGRAPHIC,
			PERSPECTIVE
		} Projection;

		float FOV;

		glm::vec2 ClippingPlanes;
		glm::vec2 ViewportRect;

		bool Primary;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const glm::vec4& backgroundColor, ProjectionType projectionType, float fov, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, bool primary)
			: BackgroundColor(backgroundColor), Projection(projectionType), FOV(fov), ClippingPlanes(clippingPlanes), ViewportRect(viewportRect), Primary(primary) {}
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
}
