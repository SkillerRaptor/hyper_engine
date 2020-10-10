#include "Scene.hpp"

#include "Entity.hpp"
#include "Systems/CameraControllerSystem.hpp"
#include "Systems/CharacterControllerSystem.hpp"
#include "Systems/SpriteRendererSystem.hpp"

namespace Hyperion
{
	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
		Init();
	}

	Scene::~Scene()
	{
	}

	void Scene::Init()
	{
		m_Registry = CreateRef<Registry>();
		m_Systems = CreateRef<EntitySystems>(*m_Registry);

		m_Systems->AddSystem<CameraControllerSystem>();
		m_Systems->AddSystem<CharacterControllerSystem>();
		m_Systems->AddSystem<SpriteRendererSystem>();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return Entity(m_Registry->ConstructEntity(name), this);
	}

	void Scene::DeleteEntity(Entity& entity)
	{
		m_Registry->DeleteEntity(entity.GetEntityHandle());
	}

	void Scene::OnTick(int currentTick)
	{
		m_Systems->OnTick(currentTick);
	}

	void Scene::OnRender()
	{
		m_Systems->OnRender();
	}

	void Scene::OnUpdate(Timestep timeStep)
	{
		m_Systems->OnUpdate(timeStep);
	}

	void Scene::OnEvent(Event& event)
	{
		m_Systems->OnEvent(event);
	}

	void Scene::Each(const typename std::common_type<std::function<void(Entity)>>::type function)
	{
		m_Registry->Each([&](uint32_t entity)
			{
				function(Entity(entity, this));
			});
	}

	Registry& Scene::GetRegistry()
	{
		return *m_Registry;
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
