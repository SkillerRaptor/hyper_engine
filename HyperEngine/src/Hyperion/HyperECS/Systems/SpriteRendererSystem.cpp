#include "SpriteRendererSystem.hpp"

namespace Hyperion
{
	SpriteRendererSystem::SpriteRendererSystem(Ref<Renderer2D> renderer2D)
		: m_Renderer2D(renderer2D)
	{
	}

	void SpriteRendererSystem::OnRender(Registry& registry)
	{
		m_Renderer2D->BeginScene();
		registry.Each<SpriteRendererComponent, TransformComponent>([&](Entity entity, SpriteRendererComponent& spriteRenderer, TransformComponent& transform)
			{
				m_Renderer2D->DrawQuad(transform.GetPosition(), transform.GetRotation(), transform.GetScale(), spriteRenderer.GetColor(), spriteRenderer.GetTexture());
			});
		m_Renderer2D->EndScene();
	}
}
