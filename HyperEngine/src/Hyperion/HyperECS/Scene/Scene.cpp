#include "Scene.hpp"

#include "HyperECS/HyperEntity.hpp"
#include "HyperECS/Systems/CharacterControllerSystem.hpp"
#include "HyperECS/Systems/SpriteRendererSystem.hpp"

namespace Hyperion
{
	Scene::Scene(const std::string& name, Ref<Renderer2D> renderer2D)
		: m_Name(name), m_Renderer2D(renderer2D)
	{
		Init();
	}

	Scene::~Scene()
	{
	}

	void Scene::Init()
	{
		m_World = CreateRef<World>(m_Renderer2D);

		m_World->AddSystem<CharacterControllerSystem>();
		m_World->AddSystem<SpriteRendererSystem>();
	}

	HyperEntity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = m_World->Construct();
		m_World->AddComponent<TransformComponent>(entity, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(10.0f, 10.0f, 10.0f));
		m_World->AddComponent<TagComponent>(entity, name.empty() ? "Entity" : name);
		return HyperEntity(entity, this);
	}

	void Scene::DeleteEntity(HyperEntity& entity)
	{
		m_World->Destroy(entity.GetEntityHandle());
	}

	void Scene::OnRender()
	{
		m_World->OnRender();
	}

	void Scene::OnUpdate(Timestep timeStep)
	{
		m_World->OnUpdate(timeStep);
	}

	void Scene::OnEvent(Event& event)
	{
		m_World->OnEvent(event);
	}

	void Scene::Each(const typename std::common_type<std::function<void(HyperEntity)>>::type function)
	{
		m_World->Each([&](Entity entity)
			{
				function(HyperEntity(entity, this));
			});
	}

	World& Scene::GetWorld()
	{
		return *m_World;
	}

	void Scene::SetName(const std::string& name)
	{
		m_Name = name;
	}

	std::string Scene::GetName() const
	{
		return m_Name;
	}
}
