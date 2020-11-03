#pragma once

#include "Registry.hpp"
#include "HyperEvents/HyperEvents.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class EntitySystem
	{
	protected:
		Renderer2D* m_Renderer2D;

	public:
		virtual void Tick(Registry& registry, int currentTick) {};
		virtual void Update(Registry& registry, Timestep timeStep) {};
		virtual void Render(Registry& registry) {};

		virtual void OnEvent(Registry& registry, Event& event) {};

		virtual void SetRenderer2D(Renderer2D* renderer) { m_Renderer2D = renderer; }
	};
}
