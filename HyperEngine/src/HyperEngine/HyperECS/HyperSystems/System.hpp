#pragma once

#include "HyperECS/Registry.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace HyperECS
{
	class System
	{
	public:
		System() = default;
		virtual ~System() = default;

		virtual void OnUpdate(Registry& registry, HyperUtilities::Timestep timeStep) {}
		virtual void OnRender(Registry& registry) {}
	};
}
