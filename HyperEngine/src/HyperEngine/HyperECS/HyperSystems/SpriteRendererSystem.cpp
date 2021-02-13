#include "HyperECS/HyperSystems/SpriteRendererSystem.hpp"

namespace HyperECS
{
	SpriteRendererSystem::SpriteRendererSystem(HyperCore::Ref<HyperRendering::Renderer2D> renderer2D)
		: m_Renderer2D{ renderer2D }
	{
	}

	void SpriteRendererSystem::OnRender(Registry& registry)
	{
		m_Renderer2D->BeginScene();
		for (Entity entity : registry.Each<SpriteRendererComponent, TransformComponent>())
		{
			SpriteRendererComponent& spriteRenderer = registry.GetComponent<SpriteRendererComponent>(entity);
			TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
			m_Renderer2D->DrawQuad(transform.GetPosition(), transform.GetRotation(), transform.GetScale(), spriteRenderer.GetColor(), spriteRenderer.GetTexture());
		}
		m_Renderer2D->EndScene();
	}
}
