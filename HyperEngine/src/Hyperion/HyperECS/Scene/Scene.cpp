#include "Scene.hpp"

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
		if (m_Registry)
			delete m_Registry;
	}

	void Scene::Init()
	{
		m_Registry = new Registry();

		m_Systems.push_back(std::move(new SpriteRendererSystem(m_Renderer2D)));
	}

	void Scene::Clear()
	{
		if (m_Registry)
			delete m_Registry;

		Init();
	}

	void Scene::OnRender()
	{
		for (System* system : m_Systems)
			system->OnRender(*m_Registry);
	}

	void Scene::OnUpdate(Timestep timeStep)
	{
		for (System* system : m_Systems)
			system->OnUpdate(*m_Registry, timeStep);
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = m_Registry->Create();
		m_Registry->AddComponent<TagComponent>(entity, name);
		m_Registry->AddComponent<TransformComponent>(entity, glm::vec3{ 0.0f }, glm::vec3{ 0.0f }, glm::vec3{ 1.0f });
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry->Destroy(entity);
	}

	void Scene::SetName(const std::string& name)
	{
		m_Name = name;
	}

	std::string Scene::GetName() const
	{
		return m_Name;
	}

	Registry& Scene::GetRegistry()
	{
		return *m_Registry;
	}
}
