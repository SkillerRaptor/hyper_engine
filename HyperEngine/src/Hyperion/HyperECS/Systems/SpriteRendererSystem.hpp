#pragma once

#include "HyperECS/HyperECS.hpp"
#include "HyperECS/Components/Components.hpp"

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
					m_Renderer2D->DrawQuad(transform.GetPosition(), transform.GetRotation(), transform.GetScale(), spriteRenderer.GetColor());
				});
			m_Renderer2D->EndScene();
		}
	};
}
