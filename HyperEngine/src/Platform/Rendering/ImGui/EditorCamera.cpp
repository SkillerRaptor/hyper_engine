#include "EditorCamera.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "HyperEvents/MouseEvents.hpp"
#include "HyperUtilities/Input.hpp"

namespace Hyperion
{
	EditorCamera::EditorCamera(const glm::vec3& position, const glm::vec2& viewportSize, float speed, float zoom, float zoomSpeed, float nearPlane, float farPlane, float yaw, float pitch, CameraTypeInfo cameraTypeInfo)
		: m_Position(position), m_ViewportSize(viewportSize), m_Speed(speed), m_Zoom(zoom), m_ZoomSpeed(zoomSpeed), m_NearPlane(nearPlane), m_FarPlane(farPlane), m_Yaw(yaw), m_Pitch(pitch), m_CameraType(cameraTypeInfo), m_Front(glm::vec3(0.0f, 0.0f, 100.0f)), m_MouseSenitivity(0.1f)
	{
		UpdateProjectionVectors();
	}

	void EditorCamera::OnUpdate(Timestep timeStep)
	{
		if (m_Selected)
		{
			switch (m_CameraType)
			{
			case CameraTypeInfo::ORTHOGRAPHIC:
			{
				m_Position.x -= (float)(m_Speed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
				m_Position.y += (float)(m_Speed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
				break;
			}
			case CameraTypeInfo::PROJECTION:
			{
				if (Input::IsMouseButtonPressed(MouseCode::ButtonLeft))
				{
					float xOffset = Input::GetMouseX() - m_LastMousePosition.x;
					float yOffset = m_LastMousePosition.y - Input::GetMouseY();

					xOffset *= m_MouseSenitivity;
					yOffset *= m_MouseSenitivity;

					m_Yaw += xOffset;
					m_Pitch += yOffset;

					if (m_Pitch > 89.0f)
						m_Pitch = 89.0f;
					if (m_Pitch < -89.0f)
						m_Pitch = -89.0f;

					UpdateProjectionVectors();
				}

				m_LastMousePosition.x = Input::GetMouseX();
				m_LastMousePosition.y = Input::GetMouseY();

				float velocity = static_cast<float>(m_Speed * timeStep);
				if (Input::GetAxis(InputAxis::VERTICAL) == 1)
					m_Position += m_Front * velocity;
				if (Input::GetAxis(InputAxis::VERTICAL) == -1)
					m_Position -= m_Front * velocity;
				if (Input::GetAxis(InputAxis::HORIZONTAL) == 1)
					m_Position -= m_Right * velocity;
				if (Input::GetAxis(InputAxis::HORIZONTAL) == -1)
					m_Position += m_Right * velocity;
				break;
			}
			default:
				break;
			}

			UpdateViewMatrix();
			UpdateProjectionMatrix();
		}
	}

	void EditorCamera::OnEvent(Event& event)
	{
		if (m_Selected)
		{
			EventDispatcher dispatcher(event);

			dispatcher.Dispatch<MouseScrolledEvent>([&](MouseScrolledEvent& event)
				{
					float& zoom = m_Zoom;
					zoom -= event.GetYOffset() * m_ZoomSpeed;
					zoom = (zoom < m_ZoomSpeed) ? m_ZoomSpeed : zoom;

					UpdateProjectionMatrix();

					return false;
				});
		}
	}

	void EditorCamera::UpdateProjectionVectors()
	{
		glm::vec3 front;
		front.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
		front.y = glm::sin(glm::radians(m_Pitch));
		front.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);

		m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}

	void EditorCamera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		UpdateViewMatrix();
	}

	const glm::vec3& EditorCamera::GetPosition() const
	{
		return m_Position;
	}

	void EditorCamera::SetViewportSize(const glm::vec2& viewportSize)
	{
		m_ViewportSize = viewportSize;
		UpdateProjectionMatrix();
	}

	const glm::vec2& EditorCamera::GetViewportSize() const
	{
		return m_ViewportSize;
	}

	void EditorCamera::SetZoom(float zoom)
	{
		m_Zoom = zoom;
		UpdateProjectionMatrix();
	}

	float EditorCamera::GetZoom() const
	{
		return m_Zoom;
	}

	void EditorCamera::SetSpeed(float speed)
	{
		m_Speed = speed;
	}

	float EditorCamera::GetSpeed() const
	{
		return m_Speed;
	}

	void EditorCamera::SetNearPlane(float nearPlane)
	{
		m_NearPlane = nearPlane;
		UpdateProjectionMatrix();
	}

	float EditorCamera::GetNearPlane() const
	{
		return m_NearPlane;
	}

	void EditorCamera::SetFarPlane(float farPlane)
	{
		m_FarPlane = farPlane;
		UpdateProjectionMatrix();
	}

	float EditorCamera::GetFarPlane() const
	{
		return m_FarPlane;
	}

	void EditorCamera::UpdateProjectionMatrix()
	{
		m_ShaderManager->UseShader({ 0 });
		m_ShaderManager->SetMatrix4({ 0 }, "u_ProjectionMatrix", GetProjectionMatrix());
	}

	glm::mat4 EditorCamera::GetProjectionMatrix() const
	{
		float aspectRatio = (float)m_ViewportSize.x / m_ViewportSize.y;
		switch (m_CameraType)
		{
		case CameraTypeInfo::ORTHOGRAPHIC:
			return glm::ortho(-aspectRatio * m_Zoom, aspectRatio * m_Zoom, -m_Zoom, m_Zoom, m_NearPlane, m_FarPlane);
		case CameraTypeInfo::PROJECTION:
			return glm::perspective(glm::radians(m_Zoom), aspectRatio, m_NearPlane, m_FarPlane);
		default:
			return glm::mat4(1.0f);
		}
	}

	void EditorCamera::UpdateViewMatrix()
	{
		m_ShaderManager->UseShader({ 0 });
		m_ShaderManager->SetMatrix4({ 0 }, "u_ViewMatrix", GetViewMatrix());
	}

	glm::mat4 EditorCamera::GetViewMatrix() const
	{
		switch (m_CameraType)
		{
		case CameraTypeInfo::ORTHOGRAPHIC:
			return glm::translate(glm::mat4(1.0f), glm::vec3(m_Position.z, m_Position.y, m_Position.x));
		case CameraTypeInfo::PROJECTION:
			return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		default:
			return glm::mat4(1.0f);
		}
	}
}
