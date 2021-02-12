#pragma once

#include <cstdint>

namespace HyperECS
{
	using Entity = uint64_t;

	struct EntityTraits
	{
		using EntityType = uint64_t;
		using VersionType = uint32_t;

		static constexpr EntityType EntityMask = 0xFFFFFFFF;
		static constexpr EntityType VersionMask = 0xFFFFFFFF;
		static constexpr size_t EntityShift = 32u;
	};

	class NullEntity
	{
	public:
		constexpr operator Entity() const noexcept
		{
			return Entity{ EntityTraits::EntityMask };
		}

		constexpr bool operator==(const NullEntity& other) const noexcept
		{
			return true;
		}

		constexpr bool operator!=(const NullEntity& other) const noexcept
		{
			return false;
		}

		constexpr bool operator==(const Entity& other) const noexcept
		{
			return (other & EntityTraits::EntityMask) == static_cast<Entity>(*this);
		}

		constexpr bool operator!=(const Entity& other) const noexcept
		{
			return !(other == *this);
		}
	};

	inline constexpr NullEntity Null{};

	constexpr bool operator==(const Entity& entity, const NullEntity& other) noexcept
	{
		return other.operator==(entity);
	}

	constexpr bool operator!=(const Entity& entity, const NullEntity& other) noexcept
	{
		return !(other == entity);
	}
}
