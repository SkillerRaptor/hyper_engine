#include "Scene.hpp"

#include "Systems/CameraControllerSystem.hpp"
#include "Systems/CharacterControllerSystem.hpp"
#include "Systems/SpriteRendererSystem.hpp"

namespace Hyperion
{
	Scene::Scene()
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

	uint32_t Scene::CreateEntity(const std::string& name)
	{
		return m_Registry->ConstructEntity(name);
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

	Registry& Scene::GetRegistry()
	{
		return *m_Registry;
	}
}
