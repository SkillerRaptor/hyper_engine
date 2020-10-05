#include "Registry.hpp"

#include "Components.hpp"
#include "EntitySystem.hpp"

namespace Hyperion
{
	Registry::Registry()
	{
	}

	Registry::~Registry()
	{
	}

	void Registry::Tick(int currentTick)
	{
		for (EntitySystem* entitySystem : m_EntitySystems)
			entitySystem->Tick(*this, currentTick);
	}

	void Registry::Update(Timestep timeStep)
	{
		for (EntitySystem* entitySystem : m_EntitySystems)
			entitySystem->Update(*this, timeStep);
	}

	void Registry::Render()
	{
		for (EntitySystem* entitySystem : m_EntitySystems)
			entitySystem->Render(*this);
	}

	uint32_t Registry::ConstructEntity(const std::string& name)
	{
		static uint32_t entityId = 0;
		ComponentBuffer componentBuffer;
		componentBuffer.AddComponent<TransformComponent>();
		componentBuffer.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
		m_Entities.emplace(entityId++, std::move(componentBuffer));
		return entityId;
	}

	void Registry::DeleteEntity(uint32_t entity)
	{
		if (m_Entities.find(entity) == m_Entities.end())
			return;
		m_Entities.erase(entity);
	}
}