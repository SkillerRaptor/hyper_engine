#pragma once

#include "HyperCore/Core.hpp"
#include "HyperECS/HyperComponents/Components.hpp"
#include "HyperECS/HyperSystems/System.hpp"
#include "HyperRendering/Renderer2D.hpp"

namespace HyperECS
{
	class SpriteRendererSystem : public System
	{
	private:
		HyperCore::Ref<HyperRendering::Renderer2D> m_Renderer2D;

	public:
		SpriteRendererSystem(HyperCore::Ref<HyperRendering::Renderer2D> renderer2D);
		virtual ~SpriteRendererSystem() = default;

		virtual void OnRender(Registry& registry) override;
	};
}
