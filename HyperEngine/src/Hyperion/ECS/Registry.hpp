#pragma once

#include <functional>
#include <type_traits>
#include <unordered_map>

#include "ComponentBuffer.hpp"

#include "Core/Core.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class EntitySystem;

	class Registry
	{
	private:
		std::unordered_map<uint32_t, ComponentBuffer> m_Entities;
		std::vector<EntitySystem*> m_EntitySystems;

	public:
		Registry();
		~Registry();

		void Tick(int currentTick);
		void Update(Timestep timeStep);
		void Render();

		uint32_t ConstructEntity(const std::string& name = std::string());
		void DeleteEntity(uint32_t entity);

		template<class T, typename... Args>
		T& AddComponent(uint32_t entity, Args&&... args)
		{
			if (m_Entities.find(entity) == m_Entities.end())
			{
				HP_CORE_ASSERT(true, "Entity does not exist!");
				return;
			}
			return m_Entities[entity].AddComponent<T>(std::forward<Args>(args)...);
		}

		template<class T>
		void RemoveComponent(uint32_t entity)
		{
			if (m_Entities.find(entity) == m_Entities.end())
			{
				HP_CORE_ASSERT(true, "Entity does not exist!");
				return;
			}
			return m_Entities[entity].RemoveComponent<T>();
		}

		template<class T>
		T& GetComponent(uint32_t entity)
		{
			if (m_Entities.find(entity) == m_Entities.end())
			{
				HP_CORE_ASSERT(true, "Entity does not exist!");
				return;
			}
			return m_Entities[entity].GetComponent<T>();
		}

		template<class T>
		bool HasComponent(uint32_t entity)
		{
			if (m_Entities.find(entity) == m_Entities.end())
			{
				HP_CORE_ASSERT(true, "Entity does not exist!");
				return;
			}
			return m_Entities[entity].HasComponent<T>();
		}

		template<class... Components>
		void Each(const typename std::common_type<std::function<void(Components&...)>>::type function)
		{
		}
	};
}