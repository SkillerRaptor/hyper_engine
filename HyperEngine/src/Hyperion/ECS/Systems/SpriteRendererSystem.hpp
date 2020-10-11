#pragma once

#include "ECS/EntitySystem.hpp"
#include "ECS/Components.hpp"
#include "ECS/Registry.hpp"

#include "Platform/OpenGL/OpenGLRenderer2D.hpp"

namespace Hyperion
{
	class SpriteRendererSystem : public EntitySystem
	{
	public:
		SpriteRendererSystem() {};
		virtual ~SpriteRendererSystem() {};

		virtual void Update(Registry& registry, Timestep timeStep) override
		{
			registry.Each<CameraComponent, TransformComponent>([&](CameraComponent& cameraComponent, TransformComponent& transform)
				{
					OpenGLRenderer2D::BeginScene(cameraComponent.Width, cameraComponent.Height, cameraComponent.Zoom, cameraComponent.NearPlane, cameraComponent.FarPlane, transform.Position);
				});

			registry.Each<TagComponent, SpriteRendererComponent, TransformComponent>([&](TagComponent& tagComponent, SpriteRendererComponent& spriteRenderer, TransformComponent& transform)
				{
					OpenGLRenderer2D::DrawQuad(transform.Position, transform.Rotation, transform.Scale, spriteRenderer.Color);
				});
			OpenGLRenderer2D::EndScene();
		}
	};
}
