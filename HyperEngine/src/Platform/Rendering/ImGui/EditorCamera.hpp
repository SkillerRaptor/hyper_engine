#pragma once

#include "HyperCore/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperMath/Matrix.hpp"
#include "HyperMath/Vector.hpp"
#include "HyperRendering/ShaderManager.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class EditorCamera
	{
	private:
		Vec3 m_Position;
		Vec2 m_ViewportSize;

		/* Perspective */
		Vec3 m_Front;
		Vec3 m_Right;
		Vec3 m_Up;

		Vec3 m_LastMousePosition;
		bool m_FirstMouse;
		float m_MouseSenitivity;

		float m_Yaw;
		float m_Pitch;
		/* End Perspective */

		float m_Speed;
		float m_Zoom;
		float m_ZoomSpeed;
		float m_NearPlane;
		float m_FarPlane;

		bool m_Orthographic;

		Ref<ShaderManager> m_ShaderManager;

		friend class EditorLayer;

	public:
		EditorCamera(const Vec3& position, const Vec2& viewportSize, float speed, float zoom, float zoomSpeed, float nearPlane, float farPlane, float yaw, float pitch, bool orthographic);

		void OnUpdate(Timestep timeStep);
		void OnEvent(Event& event);

		void SetPosition(const Vec3& position);
		const Vec3& GetPosition() const;

		void SetViewportSize(const Vec2& viewportSize);
		const Vec2& GetViewportSize() const;

		void SetZoom(float zoom);
		float GetZoom() const;

		void SetSpeed(float speed);
		float GetSpeed() const;

		void SetNearPlane(float nearPlane);
		float GetNearPlane() const;

		void SetFarPlane(float farPlane);
		float GetFarPlane() const;

		void UpdateProjectionMatrix();
		Mat4 GetProjectionMatrix() const;

		void UpdateViewMatrix();
		Mat4 GetViewMatrix() const;
	};
}
