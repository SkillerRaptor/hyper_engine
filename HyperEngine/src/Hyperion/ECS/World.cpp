#include "World.h"

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
		//for (EntitySystem* entitySystem : m_EntitySystems)
		//	entitySystem->Tick(this, currentTick);
	}

	void World::Update(Timestep timeStep)
	{
		//for (EntitySystem* entitySystem : m_EntitySystems)
		//	entitySystem->Update(this, timeStep);
	}

	void World::Render()
	{
		//for (EntitySystem* entitySystem : m_EntitySystems)
		//	entitySystem->Render(this);
	}

	Entity* World::ConstructEntity()
	{
		Entity* entity = new Entity((unsigned int) m_Entities.size());
		m_Entities.push_back(entity);
		return entity;
	}

	void World::DeleteEntity(Entity* entity)
	{
		m_Entities.erase(std::remove(m_Entities.begin(), m_Entities.end(), entity), m_Entities.end());
	}
}