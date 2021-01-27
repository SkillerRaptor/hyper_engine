#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "HyperCore/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
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
		glm::vec3 m_Front = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Right = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };

		glm::vec3 m_LastMousePosition = { 0.0f, 0.0f, 0.0f };
		bool m_FirstMouse = true;
		float m_MouseSenitivity = 1.0f;

		float m_Yaw = 90.0f;
		float m_Pitch = 0.0f;
		/* End Perspective */

		float m_MovementSpeed;
		float m_FOV;
		float m_FOVSpeed;

		glm::vec4 m_BackgroundColor;

		Ref<RenderContext> m_RenderContext;

		friend class ImGuiLayer;

	public:
		EditorCamera(const glm::vec3& position, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, ProjectionType projection, float movementSpeed, float fov, float fovSpeed, const glm::vec4 backgroundColor = {0.15, 0.15f, 0.15f, 1.0f});

		void OnUpdate(Timestep timeStep);
		void OnEvent(Event& event);

		void SetPosition(const glm::vec3& position);
		const glm::vec3& GetPosition() const;

		void SetClippingPlanes(const glm::vec2& clippingPlanes);
		const glm::vec2& GetClippingPlanes() const;

		void SetViewportRect(const glm::vec2& viewportRect);
		const glm::vec2& GetViewportRect() const;

		void SetProjectionType(ProjectionType projectionType);
		ProjectionType GetProjectionType() const;

		void SetMovementSpeed(float movementSpeed);
		float GetMovementSpeed() const;

		void SetFOV(float fov);
		float GetFOV() const;

		void SetFOVSpeed(float fovSpeed);
		float GetFOVSpeed() const;

		void SetBackgroundColor(const glm::vec4& backgroundColor);
		const glm::vec4& GetBackgroundColor() const;

		void UpdateUniforms();
		glm::mat4 GetProjectionMatrix() const;
		glm::mat4 GetViewMatrix() const;

	private:
		void UpdateProjectionVectors();
	};
}
