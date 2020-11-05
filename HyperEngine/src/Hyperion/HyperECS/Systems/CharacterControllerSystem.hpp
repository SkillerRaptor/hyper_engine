#pragma once

#include "HyperECS/EntitySystem.hpp"
#include "HyperECS/Components.hpp"
#include "HyperECS/Registry.hpp"
#include "HyperUtilities/Input.hpp"

namespace Hyperion
{
	class CharacterControllerSystem : public EntitySystem
	{
	public:
		CharacterControllerSystem() {};
		virtual ~CharacterControllerSystem() {};

		virtual void Update(Registry& registry, Timestep timeStep) override
		{
			registry.Each<CharacterControllerComponent, TransformComponent>([&](CharacterControllerComponent& characterController, TransformComponent& transform)
				{
					transform.Position.x += (float)(characterController.Speed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
					transform.Position.y -= (float)(characterController.Speed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
				});
		}
	};
}
