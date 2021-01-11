#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "HyperCore/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/ShaderManager.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class EditorCamera
	{
	public:
		enum class CameraTypeInfo
		{
			ORTHOGRAPHIC,
			PROJECTION
		};

	private:
		glm::vec3 m_Position;
		glm::vec2 m_ViewportSize;

		/* Perspective */
		glm::vec3 m_Front;
		glm::vec3 m_Right;
		glm::vec3 m_Up;

		glm::vec3 m_LastMousePosition;
		bool m_FirstMouse = true;
		float m_MouseSenitivity;

		float m_Yaw;
		float m_Pitch;
		/* End Perspective */

		float m_Speed;
		float m_Zoom;
		float m_ZoomSpeed;
		float m_NearPlane;
		float m_FarPlane;

		CameraTypeInfo m_CameraType;

		Ref<ShaderManager> m_ShaderManager;

		bool m_Selected;

		friend class ImGuiLayer;

	public:
		EditorCamera(const glm::vec3& position, const glm::vec2& viewportSize, float speed, float zoom, float zoomSpeed, float nearPlane, float farPlane, float yaw, float pitch, CameraTypeInfo cameraTypeInfo);

		void OnUpdate(Timestep timeStep);
		void OnEvent(Event& event);

		void SetPosition(const glm::vec3& position);
		const glm::vec3& GetPosition() const;

		void SetViewportSize(const glm::vec2& viewportSize);
		const glm::vec2& GetViewportSize() const;

		void SetZoom(float zoom);
		float GetZoom() const;

		void SetSpeed(float speed);
		float GetSpeed() const;

		void SetNearPlane(float nearPlane);
		float GetNearPlane() const;

		void SetFarPlane(float farPlane);
		float GetFarPlane() const;

		void UpdateProjectionMatrix();
		glm::mat4 GetProjectionMatrix() const;

		void UpdateViewMatrix();
		glm::mat4 GetViewMatrix() const;

	private:
		void UpdateProjectionVectors();
	};
}
