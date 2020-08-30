#pragma once

#include "ECS/Components.h"
#include "ECS/EntitySystem.h"
#include "Utilities/Input.h"

namespace Hyperion
{
	class CharacterControllerSystem : public EntitySystem
	{
	public:
		CharacterControllerSystem() {};
		virtual ~CharacterControllerSystem() {};

		virtual void Update(World* world, Timestep timeStep) override
		{
			world->Each<CharacterControllerComponent, TransformComponent>([&](Entity* entity, CharacterControllerComponent characterController, TransformComponent transform)
				{
					transform.Position.x += (float)(characterController.Speed * timeStep * Input::GetAxis("Horizontal"));
					transform.Position.y -= (float)(characterController.Speed * timeStep * Input::GetAxis("Vertical"));
				});
		}
	};
}