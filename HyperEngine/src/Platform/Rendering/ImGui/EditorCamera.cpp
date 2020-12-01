#include "EditorCamera.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "HyperEvents/MouseEvents.hpp"
#include "HyperUtilities/Input.hpp"

namespace Hyperion
{
	EditorCamera::EditorCamera(const Vec3& position, const Vec2& viewportSize, float speed, float zoom, float zoomSpeed, float nearPlane, float farPlane, float yaw, float pitch, bool orthographic)
		: m_Position(position), m_ViewportSize(viewportSize), m_Speed(speed), m_Zoom(zoom), m_ZoomSpeed(zoomSpeed), m_NearPlane(nearPlane), m_FarPlane(farPlane), m_Yaw(yaw), m_Pitch(pitch), m_Orthographic(orthographic), m_Front(Vec3(0.0f, 0.0f, 1.0f)), m_MouseSenitivity(0.1f)
	{
		float yawRadians = static_cast<float>(m_Yaw * ((float)M_PI) / 180);
		float pitchRadians = static_cast<float>(m_Pitch * ((float)M_PI) / 180);

		Vec3 front;
		front.x = cos(yawRadians) * cos(pitchRadians);
		front.y = sin(pitchRadians);
		front.z = sin(yawRadians) * cos(pitchRadians);
		m_Front = Vector::Normalize(front);

		m_Right = Vector::Normalize(Vector::Cross(m_Front, Vec3(0.0f, 1.0f, 0.0f)));
		m_Up = Vector::Normalize(Vector::Cross(m_Right, m_Front));
	}

	void EditorCamera::OnUpdate(Timestep timeStep)
	{
		if (m_Orthographic)
		{
			m_Position.x -= (float)(m_Speed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
			m_Position.y += (float)(m_Speed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
		}
		else
		{
			float velocity = static_cast<float>(m_Speed * timeStep);
			if (Input::GetAxis(InputAxis::VERTICAL) == 1)
				m_Position += m_Front * velocity;
			if (Input::GetAxis(InputAxis::VERTICAL) == -1)
				m_Position -= m_Front * velocity;
			if (Input::GetAxis(InputAxis::HORIZONTAL) == 1)
				m_Position -= m_Right * velocity;
			if (Input::GetAxis(InputAxis::HORIZONTAL) == -1)
				m_Position += m_Right * velocity;
		}

		UpdateViewMatrix();
		UpdateProjectionMatrix();
	}

	void EditorCamera::OnEvent(Event& event)
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

		dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& event)
			{
				if (!m_FirstMouse)
				{
					m_LastMousePosition.x = event.GetMouseX();
					m_LastMousePosition.y = event.GetMouseY();
					m_FirstMouse = true;
				}

				float xOffset = event.GetMouseX() - m_LastMousePosition.x;
				float yOffset = m_LastMousePosition.y - event.GetMouseY();

				m_LastMousePosition.x = event.GetMouseX();
				m_LastMousePosition.y = event.GetMouseY();

				m_Yaw += (xOffset * m_MouseSenitivity);
				m_Pitch += (yOffset * m_MouseSenitivity);

				if (m_Pitch > 89.0f)
					m_Pitch = 89.0f;
				if (m_Pitch < -89.0f)
					m_Pitch = -89.0f;

				float yawRadians = static_cast<float>(m_Yaw * ((float)M_PI) / 180);
				float pitchRadians = static_cast<float>(m_Pitch * ((float)M_PI) / 180);

				Vec3 front;
				front.x = cos(yawRadians) * cos(pitchRadians);
				front.y = sin(pitchRadians);
				front.z = sin(yawRadians) * cos(pitchRadians);
				m_Front = front;

				m_Right = Vector::Normalize(Vector::Cross(m_Front, Vec3(0.0f, 1.0f, 0.0f)));
				m_Up = Vector::Normalize(Vector::Cross(m_Right, m_Front));

				return false;
			});
	}

	void EditorCamera::SetPosition(const Vec3& position)
	{
		m_Position = position;
		UpdateViewMatrix();
	}

	const Vec3& EditorCamera::GetPosition() const
	{
		return m_Position;
	}

	void EditorCamera::SetViewportSize(const Vec2& viewportSize)
	{
		m_ViewportSize = viewportSize;
		UpdateProjectionMatrix();
	}

	const Vec2& EditorCamera::GetViewportSize() const
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

	Mat4 EditorCamera::GetProjectionMatrix() const
	{
		float aspectRatio = (float)m_ViewportSize.x / m_ViewportSize.y;
		if (m_Orthographic)
			return Matrix::Ortho(-aspectRatio * m_Zoom, aspectRatio * m_Zoom, -m_Zoom, m_Zoom, m_NearPlane, m_FarPlane);
		return Matrix::Perspective(45.0f, aspectRatio, m_NearPlane, m_FarPlane);
	}

	void EditorCamera::UpdateViewMatrix()
	{
		m_ShaderManager->UseShader({ 0 });
		m_ShaderManager->SetMatrix4({ 0 }, "u_ViewMatrix", GetViewMatrix());
	}

	Mat4 EditorCamera::GetViewMatrix() const
	{
		if (m_Orthographic)
			return Matrix::Translate(Mat4(1.0f), Vector::Inverse(m_Position));
		return Matrix::LookAt(m_Position, m_Position + m_Front, m_Up);
	}
}
