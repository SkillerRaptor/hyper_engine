#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "Entity.hpp"
#include "SparseSet.hpp"

namespace Hyperion
{
	class Registry
	{
	private:
		struct PoolData
		{
			uint64_t Id;
			std::unique_ptr<SparseSet> Pool{};
		};

	private:
		std::vector<Entity> m_Entities = {};
		std::vector<PoolData> m_Pools = {};
		Entity m_Available{ Null };

	public:
		Registry();
		~Registry();

		Entity Create();
		void Destroy(const Entity entity);

		bool IsValid(const Entity entity) const;

		template<typename T, typename... Args>
		T& AddComponent(const Entity entity, Args&&... args)
		{
			if (HasComponent<T>(entity))
				throw std::exception("The provided entity has already the specified component!");

			size_t componentId = typeid(T).hash_code();
			PoolData* pool = FindPool(componentId);
			if (pool == nullptr)
			{
				pool = &m_Pools.emplace_back(PoolData{ static_cast<uint64_t>(componentId), std::make_unique<SparseSet>() });
			}
			else if (pool->Pool->Contains(entity))
			{
				throw std::exception("The provided entity exists already in the pool!");
			}

			T* component = new T{ std::forward<Args>(args)... };
			pool->Pool->Emplace(entity, (void*)std::move(component));
			return *component;
		}

		template<typename T>
		void RemoveComponent(const Entity entity)
		{
			size_t componentId = typeid(T).hash_code();
			PoolData* pool = FindPool(componentId);
			if (pool == nullptr)
				throw std::exception("The pool for specified component does not exists!");

			if (!pool->Pool->Contains(entity))
				throw std::exception("The provided entity does not exists in the pool!");

			if (!HasComponent<T>(entity))
				throw std::exception("The provided entity has not the specified component!");

			pool->Pool->Remove(entity);
		}

		void RemoveAllComponents(const Entity entity);

		template<typename T>
		T& GetComponent(const Entity entity)
		{
			size_t componentId = typeid(T).hash_code();
			PoolData* pool = FindPool(componentId);
			if (pool == nullptr)
				throw std::exception("The pool for specified component does not exists!");

			if (!HasComponent<T>(entity))
				throw std::exception("The provided entity has not the specified component!");

			return *reinterpret_cast<T*>(pool->Pool->Get(entity));
		}

		template<typename T>
		bool HasComponent(const Entity entity)
		{
			size_t componentId = typeid(T).hash_code();
			PoolData* pool = FindPool(componentId);
			if (pool == nullptr)
				return false;

			return pool->Pool->Contains(entity);
		}

		void Each(const typename std::common_type<std::function<void(Entity)>>::type function);

		template<class... T>
		void Each(const typename std::common_type<std::function<void(T&...)>>::type function)
		{
			for (const auto& entity : m_Entities)
			{
				bool shouldSkip = false;

				([&](auto* v)
					{
						using C = decltype(*v);
						if (shouldSkip)
							return;
						if (!HasComponent<C>(entity))
							shouldSkip = true;
					} ((T*)nullptr), ...);

				if (shouldSkip)
					continue;
		
				function(GetComponent<T>(entity)...);
			}
		}

		template<class... T>
		void Each(const typename std::common_type<std::function<void(Entity, T&...)>>::type function)
		{
			for (const auto& entity : m_Entities)
			{
				bool shouldSkip = false;

				([&](auto* v)
					{
						using C = decltype(*v);
						if (shouldSkip)
							return;
						if (!HasComponent<C>(entity))
							shouldSkip = true;
					} ((T*)nullptr), ...);

				if (shouldSkip)
					continue;

				function(entity, GetComponent<T>(entity)...);
			}
		}

		template<class... T>
		std::vector<Entity> Each()
		{
			std::vector<Entity> entities;
			for (const auto& entity : m_Entities)
			{
				bool shouldSkip = false;

				([&](auto* v)
					{
						using C = decltype(*v);
						if (shouldSkip)
							return;
						if (!HasComponent<C>(entity))
							shouldSkip = true;
					} ((T*)nullptr), ...);

				if (shouldSkip)
					continue;

				entities.push_back(entity);
			}
			return entities;
		}

		static EntityTraits::EntityType GetEntityId(const Entity entity) noexcept;
		static EntityTraits::VersionType GetEntityVersion(const Entity entity) noexcept;

	private:
		Entity GenerateIdentifier();
		Entity RecycleIdentifier();
		PoolData* FindPool(uint64_t id);
	};
}
