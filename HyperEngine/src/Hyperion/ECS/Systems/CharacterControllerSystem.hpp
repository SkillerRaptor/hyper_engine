#pragma once

#include "ECS/Components.hpp"
#include "ECS/EntitySystem.hpp"
#include "Utilities/Input.hpp"

namespace Hyperion
{
	class CharacterControllerSystem : public EntitySystem
	{
	public:
		CharacterControllerSystem() {};
		virtual ~CharacterControllerSystem() {};

		virtual void Update(World* world, Timestep timeStep) override
		{
			std::cout << "Update CharacterControllerSystem" << std::endl;
			world->Each<CharacterControllerComponent, TransformComponent>([&](CharacterControllerComponent& characterController, TransformComponent& transform)
				{
					transform.Position.x += (float)(characterController.Speed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
					transform.Position.y -= (float)(characterController.Speed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
				});
		}
	};
}