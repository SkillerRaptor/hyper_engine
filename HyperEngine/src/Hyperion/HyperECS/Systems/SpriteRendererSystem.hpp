#pragma once

#include "HyperCore/Core.hpp"
#include "HyperECS/Components/Components.hpp"
#include "HyperECS/Systems/System.hpp"
#include "HyperRendering/Renderer2D.hpp"

namespace Hyperion
{
	class SpriteRendererSystem : public System
	{
	private:
		Ref<Renderer2D> m_Renderer2D;

	public:
		SpriteRendererSystem(Ref<Renderer2D> renderer2D);
		virtual ~SpriteRendererSystem() = default;

		virtual void OnRender(Registry& registry) override;
	};
}
