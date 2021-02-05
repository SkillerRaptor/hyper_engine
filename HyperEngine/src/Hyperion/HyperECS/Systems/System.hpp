#pragma once

#include "HyperECS/Registry.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class System
	{
	public:
		System() = default;
		virtual ~System() = default;

		virtual void OnUpdate(Registry& registry, Timestep timeStep) {};
		virtual void OnRender(Registry& registry) {};
	};
}
