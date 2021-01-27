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
			registry.Each<CameraComponent, CameraControllerComponent, TransformComponent>([&](Entity entity, CameraComponent& cameraComponent, CameraControllerComponent& cameraController, TransformComponent& transform)
				{
					switch (cameraComponent.Projection)
					{
					case CameraComponent::ProjectionType::ORTHOGRAPHIC:
					{
						transform.Position.x += (float)(cameraController.MoveSpeed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
						transform.Position.y += (float)(cameraController.MoveSpeed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
						break;
					}
					case CameraComponent::ProjectionType::PERSPECTIVE:
					{
						break;
					}
					default:
						break;
					}
				});
		}
	};
}
