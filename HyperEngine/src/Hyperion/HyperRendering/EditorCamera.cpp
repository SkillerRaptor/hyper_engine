#include "EditorCamera.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "HyperEvents/MouseEvents.hpp"
#include "HyperUtilities/Input.hpp"

namespace Hyperion
{
	EditorCamera::EditorCamera(const glm::vec3& position, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, ProjectionType projection, float movementSpeed, float fov, float fovSpeed, const glm::vec4 backgroundColor)
		: m_Position(position), m_ClippingPlanes(clippingPlanes), m_ViewportRect(viewportRect), m_Projection(projection), m_MovementSpeed(movementSpeed), m_FOV(fov), m_FOVSpeed(fovSpeed), m_BackgroundColor(backgroundColor)
	{
		UpdateProjectionVectors();
	}

	void EditorCamera::OnUpdate(Timestep timeStep)
	{
		if (m_RenderContext->GetSceneRecorder()->IsEditorFocused())
		{
			switch (m_Projection)
			{
			case ProjectionType::ORTHOGRAPHIC:
			{
				m_Position.x -= (float)(m_MovementSpeed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
				m_Position.y += (float)(m_MovementSpeed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
				break;
			}
			case ProjectionType::PERSPECTIVE:
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

				auto velocity = static_cast<float>(m_MovementSpeed * timeStep);
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
		}

		UpdateUniforms();

		m_LastMousePosition.x = Input::GetMouseX();
		m_LastMousePosition.y = Input::GetMouseY();
	}

	void EditorCamera::OnEvent(Event& event)
	{
		if (m_RenderContext->GetSceneRecorder()->IsEditorFocused())
		{
			EventDispatcher dispatcher(event);

			dispatcher.Dispatch<MouseScrolledEvent>([&](MouseScrolledEvent& event)
				{
					m_FOV -= event.GetYOffset() * m_FOVSpeed;
					m_FOV = (m_FOV < m_FOVSpeed) ? m_FOVSpeed : m_FOV;

					UpdateUniforms();

					return false;
				});
		}
	}

	void EditorCamera::UpdateProjectionVectors()
	{
		glm::vec3 front{ 0.0f };
		front.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
		front.y = glm::sin(glm::radians(m_Pitch));
		front.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);

		m_Right = glm::normalize(glm::cross(m_Front, glm::vec3{ 0.0f, 1.0f, 0.0f }));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}

	void EditorCamera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		UpdateUniforms();
	}

	const glm::vec3& EditorCamera::GetPosition() const
	{
		return m_Position;
	}

	void EditorCamera::SetClippingPlanes(const glm::vec2& clippingPlanes)
	{
		m_ClippingPlanes = clippingPlanes;
		UpdateUniforms();
	}

	const glm::vec2& EditorCamera::GetClippingPlanes() const
	{
		return m_ClippingPlanes;
	}

	void EditorCamera::SetViewportRect(const glm::vec2& viewportRect)
	{
		m_ViewportRect = viewportRect;
		UpdateUniforms();
	}

	const glm::vec2& EditorCamera::GetViewportRect() const
	{
		return m_ViewportRect;
	}

	void EditorCamera::SetProjectionType(ProjectionType projectionType)
	{
		m_Projection = projectionType;
		UpdateUniforms();
	}

	EditorCamera::ProjectionType EditorCamera::GetProjectionType() const
	{
		return m_Projection;
	}

	void EditorCamera::SetMovementSpeed(float movementSpeed)
	{
		m_MovementSpeed = movementSpeed;
	}

	float EditorCamera::GetMovementSpeed() const
	{
		return m_MovementSpeed;
	}

	void EditorCamera::SetFOV(float fov)
	{
		m_FOV = fov;
		UpdateUniforms();
	}

	float EditorCamera::GetFOV() const
	{
		return m_FOV;
	}

	void EditorCamera::SetFOVSpeed(float fovSpeed)
	{
		m_FOVSpeed = fovSpeed;
	}

	float EditorCamera::GetFOVSpeed() const
	{
		return m_FOVSpeed;
	}

	void EditorCamera::SetBackgroundColor(const glm::vec4& backgroundColor)
	{
		m_BackgroundColor = backgroundColor;
	}

	const glm::vec4& EditorCamera::GetBackgroundColor() const
	{
		return m_BackgroundColor;
	}

	void EditorCamera::UpdateUniforms()
	{
		m_RenderContext->GetRenderer2D()->SetCamera(GetProjectionMatrix(), GetViewMatrix());
	}

	glm::mat4 EditorCamera::GetProjectionMatrix() const
	{
		float aspectRatio = 0.0f;
		TextureHandle editorColorTexture = m_RenderContext->GetSceneRecorder()->GetEditorColorAttachment();

		if (editorColorTexture.IsHandleValid())
		{
			TextureData* editorColorTextureData = m_RenderContext->GetTextureManager()->GetTextureData(editorColorTexture);
			aspectRatio = (static_cast<float>(editorColorTextureData->Width) * m_ViewportRect.x) / (static_cast<float>(editorColorTextureData->Height) * m_ViewportRect.y);
		}

		switch (m_Projection)
		{
		case ProjectionType::ORTHOGRAPHIC:
			return glm::ortho(-aspectRatio * m_FOV, aspectRatio * m_FOV, -m_FOV, m_FOV, m_ClippingPlanes.x, m_ClippingPlanes.y);
		case ProjectionType::PERSPECTIVE:
			return glm::perspective(glm::radians(m_FOV), aspectRatio, m_ClippingPlanes.x, m_ClippingPlanes.y);
		default:
			return glm::mat4(1.0f);
		}
	}

	glm::mat4 EditorCamera::GetViewMatrix() const
	{
		switch (m_Projection)
		{
		case ProjectionType::ORTHOGRAPHIC:
			return glm::translate(glm::mat4(1.0f), glm::vec3(m_Position.z, m_Position.y, m_Position.x));
		case ProjectionType::PERSPECTIVE:
			return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		default:
			return glm::mat4(1.0f);
		}
	}
}
