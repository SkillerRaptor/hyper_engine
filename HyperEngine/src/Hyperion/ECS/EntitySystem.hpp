#pragma once

#include "ECS/Registry.hpp"
#include "Events/Event.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class EntitySystem
	{
	public:
		virtual void Tick(Registry& registry, int currentTick) {};
		virtual void Update(Registry& registry, Timestep timeStep) {};
		virtual void Render(Registry& registry) {};

		virtual void OnEvent(Event& event) {};
	};
}
