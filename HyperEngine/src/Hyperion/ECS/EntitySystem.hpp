#pragma once

#include "World.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class EntitySystem
	{
	public:
		EntitySystem();
		~EntitySystem();

		virtual void Tick(World* world, int currentTick) {};
		virtual void Update(World* world, Timestep timeStep) {};
		virtual void Render(World* world) {};
	};
}