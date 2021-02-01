#pragma once

#include <glm/glm.hpp>

namespace Hyperion
{
	class CameraComponent
	{
	public:
		enum class ProjectionType
		{
			ORTHOGRAPHIC,
			PERSPECTIVE
		};

	private:
		glm::vec4 m_BackgroundColor;

		ProjectionType m_Projection;

		float m_FieldOfView;

		glm::vec2 m_ClippingPlanes;
		glm::vec2 m_ViewportRect;

		bool m_Primary;

	public:
		CameraComponent() = default;
		~CameraComponent() = default;

		CameraComponent(const glm::vec4& backgroundColor, ProjectionType projectionType, float fieldOfView, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, bool primary);

		CameraComponent(const CameraComponent& other);
		CameraComponent& operator=(const CameraComponent& other);

		CameraComponent(CameraComponent&& other) noexcept;
		CameraComponent& operator=(CameraComponent&& other) noexcept;

		bool operator==(const CameraComponent& other) const;
		bool operator!=(const CameraComponent& other) const;

		CameraComponent& SetBackgroundColor(const glm::vec4& backgroundColor);
		const glm::vec4& GetBackgroundColor() const;

		CameraComponent& SetProjection(ProjectionType projection);
		ProjectionType GetProjection() const;

		CameraComponent& SetFieldOfView(float fieldOfView);
		float GetFieldOfView() const;

		CameraComponent& SetClippingPlanes(const glm::vec2& clippingPlanes);
		const glm::vec2& GetClippingPlanes() const;

		CameraComponent& SetViewportRect(const glm::vec2& viewportRect);
		const glm::vec2& GetViewportRect() const;

		CameraComponent& SetPrimary(bool primary);
		bool IsPrimary() const;
	};
}
