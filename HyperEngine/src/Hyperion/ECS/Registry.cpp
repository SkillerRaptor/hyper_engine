#include "Registry.hpp"

#include <limits>

#include "Components.hpp"
#include "Entity.hpp"
#include "Utilities/Random.hpp"

namespace Hyperion
{
	Registry::Registry()
	{
	}

	Registry::~Registry()
	{
	}

	uint32_t Registry::ConstructEntity(const std::string& name)
	{
		uint32_t hashedName = Hasher::PrimeHasher(name);
		uint32_t entity = m_Entities.emplace_back(hashedName + Random::Int(1, (std::numeric_limits<uint32_t>::max)() - hashedName - 1));

		AddComponent<TransformComponent>(entity, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(10.0f, 10.0f, 10.0f));
		AddComponent<TagComponent>(entity, name.empty() ? "Entity" : name);

		return entity;
	}

	void Registry::DeleteEntity(uint32_t entity)
	{
		auto& it = std::find(m_Entities.begin(), m_Entities.end(), entity);
		if (it == m_Entities.end())
			return;

		// TODO: Remove All Components from Buffer

		m_Entities.erase(it);
		m_ComponentIndex.erase(entity);
	}
}
