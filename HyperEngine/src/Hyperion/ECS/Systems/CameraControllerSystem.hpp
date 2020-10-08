#pragma once

#include "ECS/EntitySystem.hpp"
#include "ECS/Components.hpp"
#include "ECS/Registry.hpp"
#include "Utilities/Input.hpp"

namespace Hyperion
{
	class CameraControllerSystem : public EntitySystem
	{
	public:
		CameraControllerSystem() {};
		virtual ~CameraControllerSystem() {};

		virtual void Update(Registry& registry, Timestep timeStep) override
		{
			registry.Each<CameraControllerComponent, TransformComponent>([&](CameraControllerComponent& cameraController, TransformComponent& transform)
				{
					transform.Position.x += (float)(cameraController.Speed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
					transform.Position.y -= (float)(cameraController.Speed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
				});
		}
	};
}
