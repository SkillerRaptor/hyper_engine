/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Prerequisites.hpp>

#include <cstdint>
#include <cstddef>

namespace HyperGame
{
	using Entity = uint64_t;

	struct EntityTraits
	{
		using EntityType = uint64_t;
		using IdType = uint32_t;
		using VersionType = uint32_t;
		
		static constexpr size_t entity_mask{ 0xFFFFFFFF };
		static constexpr size_t version_mask{ 0xFFFFFFFF };
		static constexpr size_t entity_shift{ 32U };
	};
	
	class NullEntity
	{
	public:
		constexpr operator Entity() const noexcept
		{
			return Entity{ EntityTraits::entity_mask };
		}
		
		constexpr auto operator==(const NullEntity& other) const noexcept -> bool
		{
			HYPERENGINE_VARIABLE_NOT_USED(other);
			return true;
		}

		constexpr auto operator!=(const NullEntity& other) const noexcept -> bool
		{
			HYPERENGINE_VARIABLE_NOT_USED(other);
			return false;
		}

		constexpr auto operator==(const Entity& other) const noexcept -> bool
		{
			return (other & EntityTraits::entity_mask) == static_cast<Entity>(*this);
		}

		constexpr auto operator!=(const Entity& other) const noexcept -> bool
		{
			return other != *this;
		}
	};

	constexpr auto operator==(const Entity& entity, const NullEntity& other) noexcept -> bool
	{
		return other.operator==(entity);
	}

	constexpr auto operator!=(const Entity& entity, const NullEntity& other) noexcept -> bool
	{
		return !(other == entity);
	}
	
	constexpr NullEntity g_null_entity{};
} // namespace HyperGame
