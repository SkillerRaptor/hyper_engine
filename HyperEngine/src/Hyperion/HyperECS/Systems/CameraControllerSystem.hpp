#pragma once

#include "HyperECS/EntitySystem.hpp"
#include "HyperECS/Components.hpp"
#include "HyperECS/Registry.hpp"
#include "Events/MouseEvents.hpp"
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
			registry.Each<CameraControllerComponent, CameraComponent, TransformComponent>([&](CameraControllerComponent& cameraController, CameraComponent& cameraComponent, TransformComponent& transform)
				{
					transform.Position.x += (float)(cameraController.MoveSpeed * timeStep * Input::GetAxis(InputAxis::HORIZONTAL));
					transform.Position.y -= (float)(cameraController.MoveSpeed * timeStep * Input::GetAxis(InputAxis::VERTICAL));
				});
		}

		virtual void OnEvent(Registry& registry, Event& event) override
		{
			EventDispatcher dispatcher(event);
			
			dispatcher.Dispatch<MouseScrolledEvent>([&](MouseScrolledEvent& e)
				{
					registry.Each<CameraControllerComponent, CameraComponent, TransformComponent>([&](CameraControllerComponent& cameraController, CameraComponent& cameraComponent, TransformComponent& transform)
						{
							float& zoom = cameraComponent.Zoom;
							zoom -= e.GetYOffset() * cameraController.ZoomSpeed;
							zoom = (zoom < cameraController.ZoomSpeed) ? cameraController.ZoomSpeed : zoom;
						});
					return false;
				});
		}
	};
}
