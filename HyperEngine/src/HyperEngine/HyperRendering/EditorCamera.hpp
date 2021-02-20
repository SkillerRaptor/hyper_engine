#pragma once

#include <glm/glm.hpp>

#include "HyperCore/Core.hpp"
#include "HyperEvent/Event.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace HyperRendering
{
	class EditorCamera
	{
	public:
		enum class ProjectionType
		{
			ORTHOGRAPHIC,
			PERSPECTIVE
		};

	private:
		glm::vec3 m_Position;
		glm::vec2 m_ClippingPlanes;
		glm::vec2 m_ViewportRect;

		ProjectionType m_Projection;

		/* Perspective */
		glm::vec3 m_Front{ 0.0f };
		glm::vec3 m_Right{ 0.0f };
		glm::vec3 m_Up{ 0.0f, 1.0f, 0.0f };

		glm::vec3 m_LastMousePosition{ 0.0f };
		bool m_FirstMouse{ true };
		float m_MouseSenitivity{ 1.0f };

		float m_Yaw{ 90.0f };
		float m_Pitch{ 0.0f };
		/* End Perspective */

		float m_MovementSpeed;
		float m_FieldOfView;
		float m_FieldOfViewSpeed;

		glm::vec4 m_BackgroundColor;

		HyperCore::Ref<RenderContext> m_RenderContext;

		friend class ImGuiLayer;

	public:
		EditorCamera(const glm::vec3& position, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, ProjectionType projection, float movementSpeed, float fov, float fovSpeed, const glm::vec4 backgroundColor = { 0.15, 0.15f, 0.15f, 1.0f });

		void OnUpdate(HyperUtilities::Timestep timeStep);

		void RegisterEvents(HyperEvent::EventManager& eventManager);

		inline void SetPosition(const glm::vec3& position)
		{
			m_Position = position;
			UpdateUniforms();
		}

		inline const glm::vec3& GetPosition() const
		{
			return m_Position;
		}

		inline void SetClippingPlanes(const glm::vec2& clippingPlanes)
		{
			m_ClippingPlanes = clippingPlanes;
			UpdateUniforms();
		}

		inline const glm::vec2& GetClippingPlanes() const
		{
			return m_ClippingPlanes;
		}

		inline void SetViewportRect(const glm::vec2& viewportRect)
		{
			m_ViewportRect = viewportRect;
			UpdateUniforms();
		}

		inline const glm::vec2& GetViewportRect() const
		{
			return m_ViewportRect;
		}

		inline void SetProjectionType(ProjectionType projectionType)
		{
			m_Projection = projectionType;
			UpdateUniforms();
		}

		inline ProjectionType GetProjectionType() const
		{
			return m_Projection;
		}

		inline void SetMovementSpeed(float movementSpeed)
		{
			m_MovementSpeed = movementSpeed;
		}

		inline float GetMovementSpeed() const
		{
			return m_MovementSpeed;
		}

		inline void SetFOV(float fov)
		{
			m_FieldOfView = fov;
			UpdateUniforms();
		}

		inline float GetFOV() const
		{
			return m_FieldOfView;
		}

		inline void SetFOVSpeed(float fovSpeed)
		{
			m_FieldOfViewSpeed = fovSpeed;
		}

		inline float GetFOVSpeed() const
		{
			return m_FieldOfViewSpeed;
		}

		inline void SetBackgroundColor(const glm::vec4& backgroundColor)
		{
			m_BackgroundColor = backgroundColor;
		}

		inline const glm::vec4& GetBackgroundColor() const
		{
			return m_BackgroundColor;
		}

		void UpdateUniforms();
		glm::mat4 GetProjectionMatrix() const;
		glm::mat4 GetViewMatrix() const;

	private:
		void UpdateProjectionVectors();
	};
}
