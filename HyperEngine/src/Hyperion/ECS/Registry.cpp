#include "Registry.hpp"

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
			entitySystem->Tick(this, currentTick);
	}

	void Registry::Update(Timestep timeStep)
	{
		for (EntitySystem* entitySystem : m_EntitySystems)
			entitySystem->Update(this, timeStep);
	}

	void Registry::Render()
	{
		for (EntitySystem* entitySystem : m_EntitySystems)
			entitySystem->Render(this);
	}

	uint32_t Registry::ConstructEntity()
	{
		return -1;
	}

	void Registry::DeleteEntity(uint32_t entity)
	{
	}
}