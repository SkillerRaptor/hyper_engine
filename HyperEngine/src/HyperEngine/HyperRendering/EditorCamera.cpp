#include "HyperRendering/EditorCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#include "HyperEvent/MouseEvents.hpp"
#include "HyperUtilities/Input.hpp"

namespace HyperRendering
{
	EditorCamera::EditorCamera(const glm::vec3& position, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, ProjectionType projection, float movementSpeed, float fov, float fovSpeed, const glm::vec4 backgroundColor)
		: m_Position{ position }, m_ClippingPlanes{ clippingPlanes }, m_ViewportRect{ viewportRect }, m_Projection{ projection }, m_MovementSpeed{ movementSpeed }, m_FieldOfView{ fov }, m_FieldOfViewSpeed{ fovSpeed }, m_BackgroundColor{ backgroundColor }
	{
		UpdateProjectionVectors();
	}

	void EditorCamera::OnUpdate(HyperUtilities::Timestep timeStep)
	{
		if (m_RenderContext->GetSceneRecorder()->IsEditorFocused())
		{
			switch (m_Projection)
			{
			case ProjectionType::ORTHOGRAPHIC:
			{
				m_Position.x -= (float)(m_MovementSpeed * timeStep * HyperUtilities::Input::GetAxis(HyperUtilities::InputAxis::HORIZONTAL));
				m_Position.y += (float)(m_MovementSpeed * timeStep * HyperUtilities::Input::GetAxis(HyperUtilities::InputAxis::VERTICAL));
				break;
			}
			case ProjectionType::PERSPECTIVE:
			{
				if (HyperUtilities::Input::IsMouseButtonPressed(HyperUtilities::MouseCode::ButtonLeft))
				{
					float xOffset = HyperUtilities::Input::GetMouseX() - m_LastMousePosition.x;
					float yOffset = m_LastMousePosition.y - HyperUtilities::Input::GetMouseY();

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
				if (HyperUtilities::Input::GetAxis(HyperUtilities::InputAxis::VERTICAL) == 1)
					m_Position += m_Front * velocity;
				if (HyperUtilities::Input::GetAxis(HyperUtilities::InputAxis::VERTICAL) == -1)
					m_Position -= m_Front * velocity;
				if (HyperUtilities::Input::GetAxis(HyperUtilities::InputAxis::HORIZONTAL) == 1)
					m_Position -= m_Right * velocity;
				if (HyperUtilities::Input::GetAxis(HyperUtilities::InputAxis::HORIZONTAL) == -1)
					m_Position += m_Right * velocity;
				break;
			}
			default:
				break;
			}
		}

		UpdateUniforms();

		m_LastMousePosition.x = HyperUtilities::Input::GetMouseX();
		m_LastMousePosition.y = HyperUtilities::Input::GetMouseY();
	}

	void EditorCamera::OnEvent(HyperEvent::Event& event)
	{
		if (m_RenderContext->GetSceneRecorder()->IsEditorFocused())
		{
			HyperEvent::EventDispatcher dispatcher(event);

			dispatcher.Dispatch<HyperEvent::MouseScrolledEvent>([&](HyperEvent::MouseScrolledEvent& event)
				{
					m_FieldOfView -= event.GetYOffset() * m_FieldOfViewSpeed;
					m_FieldOfView = (m_FieldOfView < m_FieldOfViewSpeed) ? m_FieldOfViewSpeed : m_FieldOfView;

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

	void EditorCamera::UpdateUniforms()
	{
		m_RenderContext->GetRenderer2D()->SetCamera(GetProjectionMatrix(), GetViewMatrix());
	}

	glm::mat4 EditorCamera::GetProjectionMatrix() const
	{
		float aspectRatio{ 0.0f };
		TextureHandle editorColorTexture = m_RenderContext->GetSceneRecorder()->GetEditorColorAttachment();

		if (editorColorTexture.IsHandleValid())
		{
			uint32_t width = m_RenderContext->GetTextureManager()->GetWidth(editorColorTexture).value_or(0);
			uint32_t height = m_RenderContext->GetTextureManager()->GetHeight(editorColorTexture).value_or(0);
			aspectRatio = (static_cast<float>(width) * m_ViewportRect.x) / (static_cast<float>(height) * m_ViewportRect.y);
		}

		switch (m_Projection)
		{
		case ProjectionType::ORTHOGRAPHIC:
			return glm::ortho(-aspectRatio * m_FieldOfView, aspectRatio * m_FieldOfView, -m_FieldOfView, m_FieldOfView, m_ClippingPlanes.x, m_ClippingPlanes.y);
		case ProjectionType::PERSPECTIVE:
			return glm::perspective(glm::radians(m_FieldOfView), aspectRatio, m_ClippingPlanes.x, m_ClippingPlanes.y);
		default:
			return glm::mat4{ 1.0f };
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
			return glm::mat4{ 1.0f };
		}
	}
}
