/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperGame/EntitySystem/Registry.hpp>

#include <HyperCore/Assertion.hpp>

namespace HyperGame
{
	auto HyperGame::Registry::create_entity() -> Entity
	{
		return m_available_entity == g_null_entity ? generate_identifier() : recycle_identifier();
	}

	auto HyperGame::Registry::destroy_entity(Entity entity) -> bool
	{
		if (!valid_entity(entity))
		{
			return false;
		}

		const auto entity_id = static_cast<EntityTraits::IdType>(entity & EntityTraits::entity_mask);
		const auto entity_version = static_cast<EntityTraits::VersionType>(entity >> EntityTraits::entity_shift) + 1U;
		const auto version_shift = static_cast<EntityTraits::EntityType>(entity_version) << EntityTraits::entity_shift;
		m_entities[entity_id] = static_cast<EntityTraits::EntityType>(m_available_entity | version_shift);
		m_available_entity = static_cast<EntityTraits::EntityType>(entity_id);

		return true;
	}

	auto HyperGame::Registry::valid_entity(Entity entity) -> bool
	{
		const size_t index = entity & EntityTraits::entity_mask;
		if (index >= m_entities.size())
		{
			return false;
		}

		return m_entities[index] == entity;
	}

	auto Registry::generate_identifier() -> Entity
	{
		const auto entity = static_cast<EntityTraits::EntityType>(m_entities.size());
		HYPERENGINE_ASSERT(entity < EntityTraits::entity_mask);

		const auto entity_id = entity & EntityTraits::entity_mask;
		return m_entities.emplace_back(entity_id);
	}

	auto Registry::recycle_identifier() -> Entity
	{
		HYPERENGINE_ASSERT(m_available_entity != g_null_entity);

		constexpr auto version_mask = EntityTraits::version_mask << EntityTraits::entity_shift;

		const auto current_entity_id = m_available_entity & EntityTraits::entity_mask;
		m_available_entity = m_entities[current_entity_id] & EntityTraits::entity_mask;
		m_entities[current_entity_id] = static_cast<EntityTraits::EntityType>((current_entity_id & EntityTraits::entity_mask) | (m_entities[current_entity_id] & version_mask));
		
		return m_entities[current_entity_id];
	}
} // namespace HyperGame
