#include "HyperECS/Registry.hpp"

#include "HyperCore/Core.hpp"

namespace HyperECS
{
	Registry::~Registry()
	{
		for (Entity entity : m_Entities)
			RemoveAllComponents(entity);

		m_Entities.clear();
		m_Pools.clear();
	}

	Entity Registry::Create()
	{
		return m_Available == Null ? GenerateIdentifier() : RecycleIdentifier();
	}

	void Registry::Destroy(const Entity entity)
	{
		HP_ASSERT(IsValid(entity));

		RemoveAllComponents(entity);

		const EntityTraits::EntityType entityId = GetEntityId(entity);
		const EntityTraits::VersionType entityVersion = GetEntityVersion(entity) + 1u;
		m_Entities[entityId] = EntityTraits::EntityType{ m_Available | (EntityTraits::EntityType{entityVersion} << EntityTraits::EntityShift) };
		m_Available = EntityTraits::EntityType{ entityId };
	}

	bool Registry::IsValid(const Entity entity) const
	{
		const size_t position = entity & EntityTraits::EntityMask;
		return (position < m_Entities.size() && m_Entities[position] == entity && m_Entities[position] != Null);
	}

	void Registry::RemoveAllComponents(const Entity entity)
	{
		for (PoolData& poolData : m_Pools)
		{
			if (poolData.Pool->Contains(entity))
				poolData.Pool->Remove(entity);
		}
	}

	void Registry::Each(std::function<void(Entity)> function)
	{
		for (const auto& entity : m_Entities)
			function(entity);
	}

	Entity Registry::GenerateIdentifier()
	{
		HP_ASSERT(EntityTraits::EntityType{ m_Entities.size() } < EntityTraits::EntityMask);
		return m_Entities.emplace_back(EntityTraits::EntityType{ m_Entities.size() });
	}

	Entity Registry::RecycleIdentifier()
	{
		HP_ASSERT(m_Available != Null);
		const Entity current = m_Available;
		const EntityTraits::VersionType version = static_cast<EntityTraits::VersionType>(m_Entities[current] & (EntityTraits::VersionMask << EntityTraits::EntityShift));
		m_Available = EntityTraits::EntityType{ m_Entities[current] & EntityTraits::EntityMask };
		return m_Entities[current] = EntityTraits::EntityType{ current | version };
	}

	Registry::PoolData* Registry::FindPool(uint64_t id)
	{
		PoolData* pool = nullptr;
		for (PoolData& poolData : m_Pools)
		{
			if (poolData.Id != id)
				continue;

			pool = &poolData;
			break;
		}

		return pool;
	}
}
