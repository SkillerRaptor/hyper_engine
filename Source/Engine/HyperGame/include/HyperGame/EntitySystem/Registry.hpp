/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperGame/EntitySystem/ComponentPool.hpp"
#include "HyperGame/EntitySystem/ComponentType.hpp"
#include "HyperGame/EntitySystem/Entity.hpp"

#include <HyperCore/Assertion.hpp>
#include <HyperCore/Logger.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

namespace HyperEngine
{
	class Registry
	{
	public:
		auto create_entity() -> Entity;
		auto destroy_entity(Entity entity) -> bool;

		auto valid_entity(Entity entity) -> bool;

		template <typename T, typename... Args>
		auto add_component(Entity entity, Args&&... args) -> T&
		{
			constexpr auto component_id = ComponentType<T>::id();
			if (m_component_pools.find(component_id) == m_component_pools.end())
			{
				m_component_pools[component_id] = std::make_unique<ComponentPool<T>>();
			}

			if (has_component<T>(entity))
			{
				Logger::fatal("Failed to add component to entity, which already has the component");
				HYPERENGINE_ASSERT_NOT_REACHED();
			}

			auto component_pool = static_cast<ComponentPool<T>*>(m_component_pools[component_id].get());
			auto& component = component_pool->add_component(entity, std::forward<Args>(args)...);

			return component;
		}

		template <typename T>
		auto remove_component(Entity entity) -> bool
		{
			if (!has_component<T>(entity))
			{
				Logger::fatal("Failed to remove component from entity, which doesn't has the component");
				HYPERENGINE_ASSERT_NOT_REACHED();
			}

			constexpr auto component_id = ComponentType<T>::id();

			auto component_pool = static_cast<ComponentPool<T>*>(m_component_pools[component_id].get());
			component_pool->remove_component(entity);
			return true;
		}

		template <typename T>
		auto has_component(Entity entity) -> bool
		{
			constexpr auto event_id = ComponentType<T>::id();
			if (m_component_pools.find(event_id) == m_component_pools.end())
			{
				return false;
			}

			constexpr auto component_id = ComponentType<T>::id();

			auto component_pool = static_cast<ComponentPool<T>*>(m_component_pools[component_id].get());
			return component_pool->has_component(entity);
		}

		template <typename T>
		auto get_component(Entity entity) -> T&
		{
			if (!has_component<T>(entity))
			{
				Logger::fatal("Failed to get component from entity, which doesn't has the component");
				HYPERENGINE_ASSERT_NOT_REACHED();
			}

			constexpr auto component_id = ComponentType<T>::id();

			auto component_pool = static_cast<ComponentPool<T>*>(m_component_pools[component_id].get());
			return component_pool->get_component(entity);
		}

	private:
		auto generate_identifier() -> Entity;
		auto recycle_identifier() -> Entity;

	private:
		std::vector<Entity> m_entities{};
		Entity m_available_entity{ g_null_entity };

		std::unordered_map<uint32_t, std::unique_ptr<IComponentPool>> m_component_pools{};
	};
} // namespace HyperEngine
