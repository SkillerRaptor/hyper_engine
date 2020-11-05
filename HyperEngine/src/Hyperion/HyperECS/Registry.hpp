#pragma once

#include <functional>
#include <type_traits>
#include <vector>
#include <unordered_map>

#include "ComponentBuffer.hpp"

#include "Core/Core.hpp"
#include "HyperUtilities/Hasher.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class EntitySystem;

	class Registry
	{
	private:
		struct ComponentIndex
		{
			uint32_t Id;
			size_t Index;

			ComponentIndex(uint32_t componentId, size_t componentIndex)
				: Id(componentId), Index(componentIndex) {}

			bool operator==(const ComponentIndex& other) const
			{
				return Id == other.Id && Index == other.Index;
			}
		};

		std::vector<uint32_t> m_Entities;
		std::unordered_map<uint32_t, std::vector<ComponentIndex>> m_ComponentIndex;
		std::unordered_map<uint32_t, ComponentBuffer*> m_Components;

	public:
		Registry();
		~Registry();

		uint32_t ConstructEntity(const std::string& name = std::string());
		void DeleteEntity(uint32_t entity);

		template<class T, typename... Args>
		T& AddComponent(uint32_t entity, Args&&... args)
		{
			HP_CORE_ASSERT(std::find(m_Entities.begin(), m_Entities.end(), entity) != m_Entities.end(), "Entity does not exist!");
			HP_CORE_ASSERT(!HasComponent<T>(entity), "Entity already has a the Component!");
			std::pair<size_t, T&> component = AddComponentToBuffer<T>(args...);
			AddComponentIndex<T>(entity, component.first);
			return component.second;
		}

		template<class T>
		void RemoveComponent(uint32_t entity)
		{
			HP_CORE_ASSERT(std::find(m_Entities.begin(), m_Entities.end(), entity) != m_Entities.end(), "Entity does not exist!");
			HP_CORE_ASSERT(HasComponent<T>(entity), "Entity has not the Component!");
			DeleteComponentFromBuffer<T>(GetComponentIndex<T>(entity)->Index);
			RemoveComponentIndex<T>(entity);
		}

		template<class T>
		T& GetComponent(uint32_t entity)
		{
			HP_CORE_ASSERT(std::find(m_Entities.begin(), m_Entities.end(), entity) != m_Entities.end(), "Entity does not exist!");
			HP_CORE_ASSERT(HasComponent<T>(entity), "Entity has not the Component!");
			return GetComponentFromBuffer<T>(GetComponentIndex<T>(entity)->Index);
		}

		template<class T>
		bool HasComponent(uint32_t entity)
		{
			HP_CORE_ASSERT(std::find(m_Entities.begin(), m_Entities.end(), entity) != m_Entities.end(), "Entity does not exist!");
			return HasComponentIndex<T>(entity);
		}

		void RemoveAllComponents(uint32_t entity)
		{
			for (ComponentIndex index : m_ComponentIndex[entity])
				DeleteComponentFromBuffer(index.Id, index.Index);
		}

		void Each(const typename std::common_type<std::function<void(uint32_t)>>::type function)
		{
			for (auto& entity : m_Entities)
				function(entity);
		}

		template<class... T>
		void Each(const typename std::common_type<std::function<void(T&...)>>::type function)
		{
			for (std::vector<uint32_t>::reverse_iterator it = m_Entities.rbegin(); it != m_Entities.rend(); ++it)
			{
				bool shouldSkip = false;
				auto lambda = [&]<typename C>() mutable {
					if (shouldSkip)
						return;
					if (!HasComponent<C>(*it))
						shouldSkip = true;
				};
				(lambda.template operator() < T > (), ...);
				if (shouldSkip)
					continue;
				function(GetComponent<T>(*it)...);
			}
		}

		std::vector<uint32_t>& GetEntities()
		{
			return m_Entities;
		}

		template<class... T>
		std::vector<uint32_t> GetEntities()
		{
			std::vector<uint32_t> entities;
			for (auto& entity : m_Entities)
			{
				bool shouldSkip = false;
				auto lambda = [&]<typename C>() mutable {
					if (shouldSkip)
						return;
					if (!HasComponent<C>(entity))
						shouldSkip = true;
				};
				(lambda.template operator() < T > (), ...);
				if (shouldSkip)
					continue;
				entites.push_back(entity);
			}
			return entities;
		}

	private:
		template <typename T, typename... Args>
		std::pair<size_t, T&> AddComponentToBuffer(Args&&... args)
		{
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			if (m_Components.find(componentId) == m_Components.end())
				m_Components[componentId] = new ComponentBuffer(sizeof(T) * 1024);

			std::pair <size_t, void*> componentPair = m_Components[componentId]->Allocate(sizeof(T));
			T* component = static_cast<T*>(componentPair.second);
			new (component) T(std::forward<Args>(args)...);
			return std::pair<size_t, T&>(componentPair.first, *component);
		}

		template <typename T>
		void DeleteComponentFromBuffer(size_t offset)
		{
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			void* c = m_Components[componentId]->Get(offset);
			T* component = static_cast<T*>(c);
			component->~T();
			m_Components[componentId]->Free(std::pair<size_t, void*>(offset, component));
		}

		void DeleteComponentFromBuffer(uint32_t componentId, size_t offset)
		{
			m_Components[componentId]->Free(std::pair<size_t, void*>(offset, m_Components[componentId]->Get(offset)));
		}

		template <typename T>
		T& GetComponentFromBuffer(size_t offset)
		{
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			void* componentPtr = m_Components[componentId]->Get(offset);
			T* component = static_cast<T*>(componentPtr);
			return *component;
		}

		template <typename T>
		void AddComponentIndex(uint32_t entity, size_t index)
		{
			std::vector<ComponentIndex>& vector = m_ComponentIndex[entity];
			vector.push_back(ComponentIndex(Hasher::PrimeHasher(typeid(T).name()), index));
		}

		template <typename T>
		void RemoveComponentIndex(uint32_t entity)
		{
			std::vector<ComponentIndex>& vector = m_ComponentIndex[entity];
			vector.erase(std::find(vector.begin(), vector.end(), *GetComponentIndex<T>(entity)));
		}

		template <typename T>
		ComponentIndex* GetComponentIndex(uint32_t entity)
		{
			std::vector<ComponentIndex>& vector = m_ComponentIndex[entity];
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			for (ComponentIndex& index : vector)
			{
				if (index.Id != componentId)
					continue;
				return &index;
			}
			return nullptr;
		}

		template <typename T>
		bool HasComponentIndex(uint32_t entity)
		{
			std::vector<ComponentIndex>& vector = m_ComponentIndex[entity];
			return GetComponentIndex<T>(entity) != nullptr;
		}
	};
}
