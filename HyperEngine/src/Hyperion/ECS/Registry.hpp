#pragma once

#include <functional>
#include <type_traits>
#include <map>
#include <unordered_map>

#include "ComponentBuffer.hpp"
#include "EnTT.hpp"

#include "Core/Core.hpp"
#include "Utilities/Hasher.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class EntitySystem;

	class Registry
	{
	private:
		std::map<EnTT, std::unordered_map<uint32_t, size_t>> m_Entities;
		std::unordered_map<uint32_t, ComponentBuffer*> m_Components;

	public:
		Registry();
		~Registry();

		EnTT ConstructEntity(const std::string& name = std::string());
		void DeleteEntity(EnTT entity);

		template<class T, typename... Args>
		T& AddComponent(EnTT entity, Args&&... args)
		{
			HP_CORE_ASSERT(m_Entities.find(entity) != m_Entities.end(), "Entity does not exist!");
			HP_CORE_ASSERT(!HasComponent<T>(entity), "Entity already has a the Component!");
			std::pair<size_t, T&> component = AddComponentToBuffer<T>(args...);
			m_Entities[entity][Hasher::PrimeHasher(typeid(T).name())] = component.first;
			return component.second;
		}

		template<class T>
		void RemoveComponent(EnTT entity)
		{
			HP_CORE_ASSERT(m_Entities.find(entity) != m_Entities.end(), "Entity does not exist!");
			HP_CORE_ASSERT(HasComponent<T>(entity), "Entity has not the Component!");
			DeleteComponentFromBuffer<T>(m_Entities[entity][Hasher::PrimeHasher(typeid(T).name())]);
			m_Entities[entity].erase(Hasher::PrimeHasher(typeid(T).name()));
		}

		template<class T>
		T& GetComponent(EnTT entity)
		{
			HP_CORE_ASSERT(m_Entities.find(entity) != m_Entities.end(), "Entity does not exist!");
			HP_CORE_ASSERT(HasComponent<T>(entity), "Entity has not the Component!");
			return GetComponentFromBuffer<T>(m_Entities[entity][Hasher::PrimeHasher(typeid(T).name())]);
		}

		template<class T>
		bool HasComponent(EnTT entity)
		{
			HP_CORE_ASSERT(m_Entities.find(entity) != m_Entities.end(), "Entity does not exist!");
			std::unordered_map<uint32_t, size_t> components = m_Entities[entity];
			return components.find(Hasher::PrimeHasher(typeid(T).name())) != components.end();
		}

		void Each(const typename std::common_type<std::function<void(EnTT)>>::type function)
		{
			for (auto& entity : m_Entities)
				function(entity.first);
		}

		template<class... T>
		void Each(const typename std::common_type<std::function<void(T&...)>>::type function)
		{
			for (auto& entity : m_Entities)
			{
				bool shouldSkip = false;
				auto lambda = [&]<typename C>() mutable {
					if (shouldSkip)
						return;
					if (!HasComponent<C>(entity.first))
						shouldSkip = true;
				};
				(lambda.template operator() < T > (), ...);
				if (shouldSkip)
					continue;
				function(GetComponent<T>(entity.first)...);
			}
		}

		std::vector<EnTT> GetEntities()
		{
			std::vector<EnTT> entities;
			for (auto& entity : m_Entities)
				entities.push_back(entity.first);
			return entities;
		}

		template<class... T>
		std::vector<EnTT> GetEntities()
		{
			std::vector<EnTT> entities;
			for (auto& entity : m_Entities)
			{
				bool shouldSkip = false;
				auto lambda = [&]<typename C>() mutable {
					if (shouldSkip)
						return;
					if (!HasComponent<C>(entity.first))
						shouldSkip = true;
				};
				(lambda.template operator() < T > (), ...);
				if (shouldSkip)
					continue;
				entites.push_back(entity.first);
			}
			return entities;
		}

	private:
		template <typename T, typename... Args>
		std::pair<size_t, T&> AddComponentToBuffer(Args&&... args)
		{
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			if (m_Components.find(componentId) == m_Components.end())
				m_Components[componentId] = new ComponentBuffer(sizeof(T) * 500);

			std::pair <size_t, void*> componentPair = m_Components[componentId]->Allocate(sizeof(T));
			T* component = static_cast<T*>(componentPair.second);
			new (component) T(std::forward<Args>(args)...);
			return std::pair<size_t, T&>(componentPair.first, *component);
		}

		template <typename T>
		void DeleteComponentFromBuffer(size_t offset)
		{
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			void* componentPair = m_Components[componentId]->Get(offset);
			T* component = static_cast<T*>(componentPair);
			component->~T();
			m_Components[componentId]->Free(std::pair<size_t, void*>(offset, component));
		}

		template <typename T>
		T& GetComponentFromBuffer(size_t offset)
		{
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			void* componentPtr = m_Components[componentId]->Get(offset);
			T* component = static_cast<T*>(componentPtr);
			return *component;
		}
	};
}
