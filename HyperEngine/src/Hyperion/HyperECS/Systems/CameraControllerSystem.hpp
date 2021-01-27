#pragma once

#include "HyperECS/Components.hpp"
#include "HyperECS/HyperECS.hpp"
#include "HyperUtilities/Input.hpp"

namespace Hyperion
{
	class CameraControllerSystem : public System
	{
	public:
		CameraControllerSystem() {};
		virtual ~CameraControllerSystem() {};

		virtual void OnUpdate(Registry& registry, Timestep timeStep) override
		{
			registry.Each<CameraControllerComponent, TransformComponent>([&](Entity entity, CameraControllerComponent& cameraController, TransformComponent& transform)
				{
					transform.Position.x += (float)(cameraController.MoveSpeed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
					transform.Position.y -= (float)(cameraController.MoveSpeed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
				});
		}
	};
}
