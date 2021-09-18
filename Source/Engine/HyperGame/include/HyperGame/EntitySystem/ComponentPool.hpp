/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperGame/EntitySystem/Entity.hpp"

#include <HyperCore/Assertion.hpp>
#include <HyperCore/Logger.hpp>

#include <unordered_map>

namespace HyperEngine
{
	class IComponentPool
	{
	public:
		virtual ~IComponentPool() = default;
	};

	template <typename T>
	class ComponentPool : public IComponentPool
	{
	public:
		template <typename... Args>
		auto add_component(Entity entity, Args&&... args) -> T&
		{
			if (has_component(entity))
			{
				Logger::fatal("Failed to add component to entity, which already has the component");
				HYPERENGINE_ASSERT_NOT_REACHED();
			}

			m_components[entity] = T{ std::forward<Args>(args)... };
			return m_components[entity];
		}

		auto remove_component(Entity entity) -> void
		{
			if (!has_component(entity))
			{
				Logger::fatal("Failed to remove component from entity, which doesn't has the component");
				HYPERENGINE_ASSERT_NOT_REACHED();
			}

			m_components.erase(entity);
		}

		auto has_component(Entity entity) -> bool
		{
			return m_components.find(entity) != m_components.end();
		}

		auto get_component(Entity entity) -> T&
		{
			if (!has_component(entity))
			{
				Logger::fatal("Failed to get component from entity, which doesn't has the component");
				HYPERENGINE_ASSERT_NOT_REACHED();
			}

			return m_components[entity];
		}

	private:
		std::unordered_map<Entity, T> m_components{};
	};
} // namespace HyperEngine
