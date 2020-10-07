#include "Scene.hpp"

#include "Systems/CharacterControllerSystem.hpp"

namespace Hyperion
{
	Scene::Scene()
		: m_Registry(), m_Systems(m_Registry)
	{
		Init();
	}

	Scene::~Scene()
	{
	}

	void Scene::Init()
	{
		m_Systems.AddSystem<CharacterControllerSystem>();
	}

	uint32_t Scene::CreateEntity(const std::string& name)
	{
		return m_Registry.ConstructEntity(name);
	}

	void Scene::OnTick(int currentTick)
	{
		m_Systems.OnTick(currentTick);
	}

	void Scene::OnRender()
	{
		m_Systems.OnRender();
	}

	void Scene::OnUpdate(Timestep timeStep)
	{
		m_Systems.OnUpdate(timeStep);
	}

	Registry& Scene::GetRegistry()
	{
		return m_Registry;
	}
}
