#pragma once

#include "EntitySystem.hpp"
#include "ECS/Components.hpp"
#include "ECS/Registry.hpp"
#include "Utilities/Input.hpp"

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