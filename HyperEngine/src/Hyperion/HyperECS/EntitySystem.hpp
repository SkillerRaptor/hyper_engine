#pragma once

#include "Registry.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class EntitySystem
	{
	protected:
		Ref<Renderer2D> m_Renderer2D;

	public:
		virtual void Tick(Registry& registry, int currentTick) {};
		virtual void Update(Registry& registry, Timestep timeStep) {};
		virtual void Render(Registry& registry) {};

		virtual void OnEvent(Registry& registry, Event& event) {};

		virtual void SetRenderer2D(Ref<Renderer2D> renderer) { m_Renderer2D = renderer; }
	};
}
