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
			registry.Each<CameraComponent, TransformComponent>([&](Entity entity, CameraComponent& cameraComponent, TransformComponent& transform)
				{
					m_Renderer2D->BeginScene(cameraComponent.Width, cameraComponent.Height, cameraComponent.Zoom, cameraComponent.NearPlane, cameraComponent.FarPlane, transform.Position);
				});

			registry.Each<TagComponent, SpriteRendererComponent, TransformComponent>([&](Entity entity, TagComponent& tagComponent, SpriteRendererComponent& spriteRenderer, TransformComponent& transform)
				{
					m_Renderer2D->DrawQuad(transform.Position, transform.Rotation, transform.Scale, spriteRenderer.Color);
				});
			m_Renderer2D->EndScene();
		}
	};
}
