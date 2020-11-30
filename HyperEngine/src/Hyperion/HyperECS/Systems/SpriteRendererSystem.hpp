#pragma once

#include "HyperECS/Components.hpp"
#include "HyperECS/HyperECS.hpp"

namespace Hyperion
{
	class SpriteRendererSystem : public System
	{
	public:
		SpriteRendererSystem() {};
		virtual ~SpriteRendererSystem() {};

		virtual void OnUpdate(Registry& registry, Timestep timeStep) override
		{
			CameraInfo cameraInfo{};
			registry.Each<CameraComponent, TransformComponent>([&](Entity entity, CameraComponent& cameraComponent, TransformComponent& transform)
				{
					cameraInfo.Position = transform.Position;
					cameraInfo.Width = cameraComponent.Width;
					cameraInfo.Height = cameraComponent.Height;
					cameraInfo.Zoom = cameraComponent.Zoom;
					cameraInfo.NearPlane = cameraComponent.NearPlane;
					cameraInfo.FarPlane = cameraComponent.FarPlane;
				});

			m_Renderer2D->BeginScene(cameraInfo);

			registry.Each<TagComponent, SpriteRendererComponent, TransformComponent>([&](Entity entity, TagComponent& tagComponent, SpriteRendererComponent& spriteRenderer, TransformComponent& transform)
				{
					m_Renderer2D->DrawQuad(transform.Position, transform.Rotation, transform.Scale, spriteRenderer.Color);
				});
			m_Renderer2D->EndScene();
		}
	};
}
