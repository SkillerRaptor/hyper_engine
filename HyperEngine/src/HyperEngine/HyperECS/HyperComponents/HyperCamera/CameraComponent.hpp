#pragma once

#include <glm/glm.hpp>

namespace HyperECS
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

		inline CameraComponent& SetBackgroundColor(const glm::vec4& backgroundColor)
		{
			m_BackgroundColor = backgroundColor;
			return *this;
		}

		inline const glm::vec4& GetBackgroundColor() const
		{
			return m_BackgroundColor;
		}

		inline CameraComponent& SetProjection(ProjectionType projection)
		{
			m_Projection = projection;
			return *this;
		}

		inline CameraComponent::ProjectionType GetProjection() const
		{
			return m_Projection;
		}

		inline CameraComponent& SetFieldOfView(float fieldOfView)
		{
			m_FieldOfView = fieldOfView;
			return *this;
		}

		inline float GetFieldOfView() const
		{
			return m_FieldOfView;
		}

		inline CameraComponent& SetClippingPlanes(const glm::vec2& clippingPlanes)
		{
			m_ClippingPlanes = clippingPlanes;
			return *this;
		}

		inline const glm::vec2& GetClippingPlanes() const
		{
			return m_ClippingPlanes;
		}

		inline CameraComponent& SetViewportRect(const glm::vec2& viewportRect)
		{
			m_ViewportRect = viewportRect;
			return *this;
		}

		inline const glm::vec2& GetViewportRect() const
		{
			return m_ViewportRect;
		}

		inline CameraComponent& SetPrimary(bool primary)
		{
			m_Primary = primary;
			return *this;
		}

		inline bool IsPrimary() const
		{
			return m_Primary;
		}
	};
}
