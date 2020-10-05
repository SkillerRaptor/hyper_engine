#include "World.hpp"

#include "EntitySystem.hpp"

namespace Hyperion
{
	World::World()
	{
	}

	World::~World()
	{
	}

	void World::Tick(int currentTick)
	{
		for (EntitySystem* entitySystem : m_EntitySystems)
			entitySystem->Tick(this, currentTick);
	}

	void World::Update(Timestep timeStep)
	{
		for (EntitySystem* entitySystem : m_EntitySystems)
			entitySystem->Update(this, timeStep);
	}

	void World::Render()
	{
		for (EntitySystem* entitySystem : m_EntitySystems)
			entitySystem->Render(this);
	}

	uint32_t World::ConstructEntity()
	{
		return -1;
	}

	void World::DeleteEntity(uint32_t entity)
	{
	}
}