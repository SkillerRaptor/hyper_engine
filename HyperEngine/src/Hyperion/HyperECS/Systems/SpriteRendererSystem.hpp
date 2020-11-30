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
			m_Renderer2D->BeginScene();
			registry.Each<SpriteRendererComponent, TransformComponent>([&](Entity entity, SpriteRendererComponent& spriteRenderer, TransformComponent& transform)
				{
					m_Renderer2D->DrawQuad(transform.Position, transform.Rotation, transform.Scale, spriteRenderer.Color);
				});
			m_Renderer2D->EndScene();
		}
	};
}
