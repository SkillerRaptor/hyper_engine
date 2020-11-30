#pragma once

#include "HyperECS/Components.hpp"
#include "HyperECS/HyperECS.hpp"
#include "HyperUtilities/Input.hpp"

namespace Hyperion
{
	class CharacterControllerSystem : public System
	{
	public:
		CharacterControllerSystem() {};
		virtual ~CharacterControllerSystem() {};

		virtual void OnUpdate(Registry& registry, Timestep timeStep) override
		{
			registry.Each<CharacterControllerComponent, TransformComponent>([&](Entity entity, CharacterControllerComponent& characterController, TransformComponent& transform)
				{
					transform.Position.x += (float)(characterController.Speed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
					transform.Position.y -= (float)(characterController.Speed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
				});
		}
	};
}
