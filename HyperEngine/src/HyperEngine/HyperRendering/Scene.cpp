#include "HyperRendering/Scene.hpp"

#include "HyperECS/HyperSystems/SpriteRendererSystem.hpp"

namespace HyperRendering
{
	Scene::Scene(const std::string& name, HyperCore::Ref<Renderer2D> renderer2D)
		: m_Name{ name }, m_Renderer2D{ renderer2D }
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
		m_Registry = new HyperECS::Registry();

		m_Systems.emplace_back(new HyperECS::SpriteRendererSystem(m_Renderer2D));
	}

	void Scene::Clear()
	{
		if (m_Registry)
			delete m_Registry;

		Init();
	}

	void Scene::OnRender()
	{
		for (HyperECS::System* system : m_Systems)
			system->OnRender(*m_Registry);
	}

	void Scene::OnUpdate(HyperUtilities::Timestep timeStep)
	{
		for (HyperECS::System* system : m_Systems)
			system->OnUpdate(*m_Registry, timeStep);
	}

	HyperECS::Entity Scene::CreateEntity(const std::string& name)
	{
		HyperECS::Entity entity{ m_Registry->Create() };
		m_Registry->AddComponent<HyperECS::TagComponent>(entity, name);
		m_Registry->AddComponent<HyperECS::TransformComponent>(entity, glm::vec3{ 0.0f }, glm::vec3{ 0.0f }, glm::vec3{ 1.0f });
		return entity;
	}

	void Scene::DestroyEntity(HyperECS::Entity entity)
	{
		m_Registry->Destroy(entity);
	}
}
