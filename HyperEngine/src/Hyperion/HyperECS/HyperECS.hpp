#pragma once

#include <functional>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <queue>

#include "HyperEvents/Event.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperUtilities/Timestep.hpp"

/* Definition for Mutex */
#ifdef HYPERECS_MUTEX
#endif /* HYPERECS_MUTEX */

namespace Hyperion
{
	/* Wrapper for entity UUID */
	struct Entity
	{
		/* Type id hash code from the entity */
		size_t Handle;

		/**
		 * @brief Checks if the entity is valid
		 *
		 * @return Returns if the entity was valid
		 */
		bool IsHandleValid() const { return Handle != 0; }

		/**
		 * @brief Compares entity to an another
		 *
		 * @return Returns if the entity is the same
		 */
		bool operator==(const Entity& other) const { return Handle == other.Handle; }
	};

	class Registry
	{
	private:
		/* Hasher to hash entity struct */
		struct EntityHasher
		{
			/**
			 * @brief Hashes the entity
			 *
			 * @param entity The entity that is getting hashed
			 *
			 * @return Returns the hash of the entity
			 */
			size_t operator()(const Entity& entity) const { return (std::hash<size_t>()(entity.Handle)); }
		};

		/* Component struct to hold data from a specified component */
		struct ComponentIndex
		{
			/* Type id hash code from the component */
			size_t Handle;

			/* Index from the component in the component map */
			size_t Index;

			ComponentIndex(size_t handle, size_t index)
				: Handle(handle), Index(index) {}

			/**
			 * @brief Checks if the component is valid
			 *
			 * @return Returns if the component was valid
			 */
			bool IsComponentValid() const { return Handle != 0 && Index != 0; }

			/**
			 * @brief Compares component to an another
			 *
			 * @return Returns if the component is the same
			 */
			bool operator==(const ComponentIndex& other) const { return Handle == other.Handle && Index == other.Index; }
		};

		/* Holds the free entries in the component map */
		std::queue<size_t> m_FreeIndex;

		/* Holds the typeid of a component struct and holds a vector of the object type class data */
		std::unordered_map<size_t, std::vector<void*>> m_Components;

		/* Holds the entities and the corresponding components */
		std::unordered_map<Entity, std::vector<ComponentIndex>, EntityHasher> m_Entities;

	#ifdef HYPERECS_MUTEX
		/* Mutex & Lock for the free entries */
		std::mutex m_FreeLock;

		/* Mutex & Lock for the components */
		std::mutex m_ComponentLock;

		/* Mutex & Lock for the entities */
		std::mutex m_EntityLock;
	#endif /* HYPERECS_MUTEX */

	public:
		/**
		 * @brief Constructing an entity in the registry
		 *
		 * @return Returns an entity
		 */
		Entity Construct()
		{
		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> entityLock(m_EntityLock);
		#endif /* HYPERECS_MUTEX */

			Entity entity = Entity({ m_Entities.size() + 1000 });
			m_Entities[entity] = {};
			return entity;
		}

		/**
		 * @brief Adding component to an entity
		 *
		 * @tparam T The component class that is getting created
		 * @tparam Args The arguments for the class
		 * @param entity The corresponding entity that the component getting assigned to
		 * @param args The arguments for the class
		 *
		 * @return Returns the created component
		 */
		template<class T, typename... Args>
		constexpr T& AddComponent(Entity entity, Args&&... args)
		{
		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> entityLock(m_EntityLock);
		#endif /* HYPERECS_MUTEX */

			if (m_Entities.find(entity) == m_Entities.end())
			{
				std::cerr << "[HyperECS] Entity does not exists!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			entityLock.unlock();
		#endif /* HYPERECS_MUTEX */

			if (HasComponent<T>(entity))
			{
				std::cerr << "[HyperECS] Entity already has the component!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			entityLock.lock();
		#endif /* HYPERECS_MUTEX */

			size_t componentId = typeid(T).hash_code();
			if (m_Components.find(componentId) == m_Components.end())
				m_Components[componentId] = {};

		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> freeLock(m_FreeLock);
			std::unique_lock<std::mutex> componentLock(m_ComponentLock);
		#endif /* HYPERECS_MUTEX */

			size_t index = 0;
			T* component;
			if (m_FreeIndex.empty())
			{
				index = m_Components[componentId].size();
				component = static_cast<T*>(m_Components[componentId].emplace_back((void*) new T(std::forward<Args>(args)...)));
			}
			else
			{
				index = m_FreeIndex.front();
				m_FreeIndex.pop();
				component = static_cast<T*>(m_Components[componentId][index] = ((void*) new T(std::forward<Args>(args)...)));
			}

			m_Entities[entity].emplace_back(componentId, index);

			return *component;
		}

		/**
		 * @brief Removing component from an entity
		 *
		 * @tparam T The component class that is getting removed
		 * @param entity The corresponding entity where component getting removed
		 */
		template<class T>
		constexpr void RemoveComponent(Entity entity)
		{
		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> entityLock(m_EntityLock);
		#endif /* HYPERECS_MUTEX */

			if (m_Entities.find(entity) == m_Entities.end())
			{
				std::cerr << "[HyperECS] Entity does not exists!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			entityLock.unlock();
		#endif /* HYPERECS_MUTEX */

			if (!HasComponent<T>(entity))
			{
				std::cerr << "[HyperECS] Entity has not the component!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			entityLock.lock();

			std::unique_lock<std::mutex> freeLock(m_FreeLock);
			std::unique_lock<std::mutex> componentLock(m_ComponentLock);
		#endif /* HYPERECS_MUTEX */

			size_t componentId = typeid(T).hash_code();
			std::vector<ComponentIndex>& components = m_Entities[entity];
			for (ComponentIndex& component : components)
				if (component.Handle == componentId)
				{
					m_Components[componentId][component.Index] = nullptr;
					m_FreeIndex.push(component.Index);
					components.erase(std::find(components.begin(), components.end(), component));
				}
		}

		/**
		 * @brief Removing multiple components from an entity
		 *
		 * @tparam T The component classes that are getting removed
		 * @param entity The corresponding entity where component getting removed
		 */
		template<class... T>
		constexpr void RemoveMultipleComponent(Entity entity)
		{
		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> entityLock(m_EntityLock);
		#endif /* HYPERECS_MUTEX */

			if (m_Entities.find(entity) == m_Entities.end())
			{
				std::cerr << "[HyperECS] Entity does not exists!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			entityLock.unlock();
		#endif /* HYPERECS_MUTEX */

			if (!HasMultipleComponent<T...>(entity))
			{
				std::cerr << "[HyperECS] Entity has not the component!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			entityLock.lock();
		#endif /* HYPERECS_MUTEX */

			auto lambda = [&]<typename C>() mutable
			{
			#ifdef HYPERECS_MUTEX
				entityLock.unlock();
			#endif /* HYPERECS_MUTEX */

				RemoveComponent<C>(entity);

			#ifdef HYPERECS_MUTEX
				entityLock.lock();
			#endif /* HYPERECS_MUTEX */
			};
			(lambda.template operator() < T > (), ...);
		}

		/**
		 * @brief Getting component from an entity
		 *
		 * @tparam T The component class that is searched for
		 * @param entity The corresponding entity that the component is assigned to
		 *
		 * @return Returns the corresponding component
		 */
		template<class T>
		constexpr T& GetComponent(Entity entity)
		{
		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> entityLock(m_EntityLock);
		#endif /* HYPERECS_MUTEX */

			if (m_Entities.find(entity) == m_Entities.end())
			{
				std::cerr << "[HyperECS] Entity does not exists!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			entityLock.unlock();
		#endif /* HYPERECS_MUTEX */

			if (!HasComponent<T>(entity))
			{
				std::cerr << "[HyperECS] Entity has not the component!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			entityLock.lock();

			std::unique_lock<std::mutex> freeLock(m_FreeLock);
			std::unique_lock<std::mutex> componentLock(m_ComponentLock);
		#endif /* HYPERECS_MUTEX */

			size_t componentId = typeid(T).hash_code();
			std::vector<ComponentIndex> components = m_Entities[entity];
			for (ComponentIndex component : components)
				if (component.Handle == componentId)
					return *static_cast<T*>(m_Components[componentId][component.Index]);
		}

		/**
		 * @brief Check if an entity has a component
		 *
		 * @tparam T The component class that is getting checked
		 * @param entity The corresponding entity that the component is assigned to
		 *
		 * @return Returns if the component was found
		 */
		template<class T>
		constexpr bool HasComponent(Entity entity)
		{
		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> entityLock(m_EntityLock);
		#endif /* HYPERECS_MUTEX */

			if (m_Entities.find(entity) == m_Entities.end())
			{
				std::cerr << "[HyperECS] Entity does not exists!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> componentLock(m_ComponentLock);
		#endif /* HYPERECS_MUTEX */

			size_t componentId = typeid(T).hash_code();
			std::vector<ComponentIndex> components = m_Entities[entity];
			for (ComponentIndex component : components)
				if (component.Handle == componentId)
					return true;
			return false;
		}

		/**
		 * @brief Check if an entity has multiple components
		 *
		 * @tparam T The component classes that are getting checked
		 * @param entity The corresponding entity that the component is assigned to
		 *
		 * @return Returns if the components were found
		 */
		template<class... T>
		constexpr bool HasMultipleComponent(Entity entity)
		{
		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> entityLock(m_EntityLock);
		#endif /* HYPERECS_MUTEX */

			if (m_Entities.find(entity) == m_Entities.end())
			{
				std::cerr << "[HyperECS] Entity does not exists!" << std::endl;
				__debugbreak();
			}

			bool shouldSkip = false;
			auto lambda = [&]<typename C>() mutable
			{
				if (shouldSkip)
					return;

			#ifdef HYPERECS_MUTEX
				entityLock.unlock();
			#endif /* HYPERECS_MUTEX */

				if (!HasComponent<C>(entity))
					shouldSkip = true;

			#ifdef HYPERECS_MUTEX
				entityLock.lock();
			#endif /* HYPERECS_MUTEX */
			};
			(lambda.template operator() < T > (), ...);
			if (shouldSkip)
				return false;
			return true;
		}

		/**
		 * @brief Calling a function for every entity
		 *
		 * @param function Function that is getting called for every entity
		 */
		void Each(const typename std::common_type<std::function<void(Entity)>>::type function)
		{
			for (auto& entity : m_Entities)
				function(entity.first);
		}

		/**
		 * @brief Calling a function for every entity with specified components
		 *
		 * @tparam T The classes that are getting filtered
		 * @param function Function that is getting called for every entity that has the specified components
		 */
		template<class... T>
		constexpr void Each(const typename std::common_type<std::function<void(Entity, T&...)>>::type function)
		{
			for (const auto& entity : m_Entities)
			{
				bool shouldSkip = false;
				auto lambda = [&]<typename C>() mutable
				{
					if (shouldSkip)
						return;
					if (!HasComponent<C>(entity.first))
						shouldSkip = true;
				};
				(lambda.template operator() < T > (), ...);
				if (shouldSkip)
					continue;
				function(entity.first, GetComponent<T>(entity.first)...);
			}
		}

		/**
		 * @brief Getting all entities
		 *
		 * @return Returns all entities
		 */
		std::vector<Entity> GetEntities() const
		{
			std::vector<Entity> entities;
			for (const auto& entity : m_Entities)
				entities.push_back(entity.first);
			return entities;
		}

		/**
		 * @brief Getting all entities with specified components
		 *
		 * @tparam T The classes that are getting filtered
		 *
		 * @return Returns all entities with specified components
		 */
		template<class... T>
		constexpr std::vector<Entity> GetEntities()
		{
			std::vector<Entity> entities;
			for (auto& entity : m_Entities)
			{
				bool shouldSkip = false;
				auto lambda = [&]<typename C>() mutable {
					if (shouldSkip)
						return;
					if (!HasComponent<C>(entity.first))
						shouldSkip = true;
				}; (lambda.template operator() < T > (), ...);
				if (shouldSkip)
					continue;
				entities.push_back(entity.first);
			}
			return entities;
		}
	};

	class System
	{
	protected:
		Ref<Renderer2D> m_Renderer2D;

		friend class World;

	public:
		/**
		 * @brief Getting called every tick
		 *
		 * @param registry The current registry where the system applies
		 * @param currentTick The current tick that is executing
		 */
		virtual void OnTick(Registry& registry, int currentTick) {};

		/**
		 * @brief Getting called every update frame
		 *
		 * @param registry The current registry where the system applies
		 * @param deltaTime The time difference between last update and current update
		 */
		virtual void OnUpdate(Registry& registry, Timestep deltaTime) {};

		/**
		 * @brief Getting called every render frame
		 *
		 * @param registry The current registry where the system applies
		 */
		virtual void OnRender(Registry& registry) {};

		/**
		 * @brief Getting called every render frame
		 *
		 * @param registry The current registry where the system applies
		 */
		virtual void OnEvent(Registry& regsitry, Event& event) {};
	};

	class World
	{
	private:
		/* Registry for the current world */
		Registry m_Registry;

		/* Map that holds the typeid of a system as UUID and the corresponding system class data */
		std::unordered_map<size_t, System*> m_Systems;

		/* Mutex & Lock for the systems */
		std::mutex m_SystemLock;

		Ref<Renderer2D> m_Renderer2D;

	public:
		World(Ref<Renderer2D> renderer2D)
			: m_Renderer2D(renderer2D) {}

		/**
		 * @brief Constructing an entity in the registry
		 *
		 * @return Returns an entity
		 */
		Entity Construct()
		{
			return m_Registry.Construct();
		}

		/**
		 * @brief Adding component to an entity
		 *
		 * @tparam T The component class that is getting created
		 * @tparam Args The arguments for the class
		 * @param entity The corresponding entity that the component getting assigned to
		 * @param args The arguments for the class
		 *
		 * @return Returns the created component
		 */
		template<class T, typename... Args>
		constexpr T& AddComponent(Entity entity, Args&&... args)
		{
			return m_Registry.AddComponent<T>(entity, args...);
		}

		/**
		 * @brief Removing component from an entity
		 *
		 * @tparam T The component class that is getting removed
		 * @param entity The corresponding entity where component getting removed
		 */
		template<class T>
		constexpr void RemoveComponent(Entity entity)
		{
			m_Registry.RemoveComponent<T>(entity);
		}

		/**
		 * @brief Removing multiple components from an entity
		 *
		 * @tparam T The component classes that are getting removed
		 * @param entity The corresponding entity where component getting removed
		 */
		template<class... T>
		constexpr void RemoveMultipleComponent(Entity entity)
		{
			m_Registry.RemoveMultipleComponent<T...>(entity);
		}

		/**
		 * @brief Getting component from an entity
		 *
		 * @tparam T The component class that is searched for
		 * @param entity The corresponding entity that the component is assigned to
		 *
		 * @return Returns the corresponding component
		 */
		template<class T>
		constexpr T& GetComponent(Entity entity)
		{
			return m_Registry.GetComponent<T>(entity);
		}

		/**
		 * @brief Check if an entity has a component
		 *
		 * @tparam T The component class that is getting checked
		 * @param entity The corresponding entity that the component is assigned to
		 *
		 * @return Returns if the component was found
		 */
		template<class T>
		constexpr bool HasComponent(Entity entity)
		{
			return m_Registry.HasComponent<T>(entity);
		}

		/**
		 * @brief Check if an entity has multiple components
		 *
		 * @tparam T The component classes that are getting checked
		 * @param entity The corresponding entity that the component is assigned to
		 *
		 * @return Returns if the components were found
		 */
		template<class... T>
		constexpr bool HasMultipleComponent(Entity entity)
		{
			return m_Registry.HasMultipleComponent<T...>(entity);
		}

		/**
		 * @brief Calling a function for every entity
		 *
		 * @param function Function that is getting called for every entity
		 */
		void Each(const typename std::common_type<std::function<void(Entity)>>::type function)
		{
			m_Registry.Each(function);
		}

		/**
		 * @brief Calling a function for every entity with specified components
		 *
		 * @tparam T The classes that are getting filtered
		 * @param function Function that is getting called for every entity that has the specified components
		 */
		template<class... T>
		constexpr void Each(const typename std::common_type<std::function<void(Entity, T&...)>>::type function)
		{
			m_Registry.Each<T...>(function);
		}

		/**
		 * @brief Getting all entities
		 *
		 * @return Returns all entities
		 */
		std::vector<Entity> GetEntities() const
		{
			return m_Registry.GetEntities();
		}

		/**
		 * @brief Getting all entities with specified components
		 *
		 * @tparam T The classes that are getting filtered
		 *
		 * @return Returns all entities with specified components
		 */
		template<class... T>
		constexpr std::vector<Entity> GetEntities()
		{
			return m_Registry.GetEntities<T...>();
		}

		/**
		 * @brief Adding system to the world
		 *
		 * @tparam T The system class that is getting created
		 * @tparam Args The arguments for the class
		 * @param args The arguments for the class
		 *
		 * @return Returns the created system
		 */
		template<class T, class = class std::enable_if<std::is_base_of<System, T>::value, T>::type, typename... Args>
		constexpr T& AddSystem(Args&&... args)
		{
			if (HasSystem<T>())
			{
				std::cerr << "[HyperECS] World already has the System!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> systemLock(m_SystemLock);
		#endif /* HYPERECS_MUTEX */

			m_Systems[typeid(T).hash_code()] = new T(std::forward<Args>(args)...);
			m_Systems[typeid(T).hash_code()]->m_Renderer2D = m_Renderer2D;
			return *static_cast<T*>(m_Systems.at(typeid(T).hash_code()));
		}

		/**
		 * @brief Removing a system from an entity
		 *
		 * @tparam T The system class that is getting removed
		 */
		template<class T, class = class std::enable_if<std::is_base_of<System, T>::value, T>::type>
		constexpr void RemoveSystem()
		{
			if (!HasSystem<T>())
			{
				std::cerr << "[HyperECS] World has not the System!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> systemLock(m_SystemLock);
		#endif /* HYPERECS_MUTEX */

			m_Systems.erase(typeid(T).hash_code());
		}

		/**
		 * @brief Removing multiple system from the world
		 *
		 * @tparam T The system classes that are getting removed
		 */
		template<class... T, class = class std::enable_if<std::is_base_of<System, T...>::value, T...>::type>
		constexpr void RemoveMultipleSystem()
		{
			if (!HasMultipleSystem<T...>())
			{
				std::cerr << "[HyperECS] World has not the System!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> systemLock(m_SystemLock);
		#endif /* HYPERECS_MUTEX */

			auto lambda = [&]<typename C>() mutable
			{
				RemoveSystem<C>();
			};
			(lambda.template operator() < T > (), ...);
		}

		/**
		 * @brief Getting the system from the world
		 *
		 * @tparam T The system class that is searched for
		 *
		 * @return Returns the corresponding system
		 */
		template<class T, class = class std::enable_if<std::is_base_of<System, T>::value, T>::type>
		constexpr T& GetSystem()
		{
			if (!HasSystem<T>())
			{
				std::cerr << "[HyperECS] World has not the System!" << std::endl;
				__debugbreak();
			}

		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> systemLock(m_SystemLock);
		#endif /* HYPERECS_MUTEX */

			return *static_cast<T*>(m_Systems.at(typeid(T).hash_code()));
		}

		/**
		 * @brief Check if the world has a system
		 *
		 * @tparam T The system class that is getting checked
		 *
		 * @return Returns if the system was found
		 */
		template<class T, class = class std::enable_if<std::is_base_of<System, T>::value, T>::type>
		constexpr bool HasSystem()
		{
		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> systemLock(m_SystemLock);
		#endif /* HYPERECS_MUTEX */

			return m_Systems.find(typeid(T).hash_code()) != m_Systems.end();
		}

		/**
		 * @brief Check if the world has multiple systems
		 *
		 * @tparam T The systems classes that are getting checked
		 *
		 * @return Returns if the systems were found
		 */
		template<class... T, class = class std::enable_if<std::is_base_of<System, T...>::value, T...>::type>
		constexpr bool HasMultipleSystem()
		{
		#ifdef HYPERECS_MUTEX
			std::unique_lock<std::mutex> systemLock(m_SystemLock);
		#endif /* HYPERECS_MUTEX */

			bool shouldSkip = false;
			auto lambda = [&]<typename C>() mutable
			{
				if (shouldSkip)
					return;

			#ifdef HYPERECS_MUTEX
				systemLock.unlock();
			#endif /* HYPERECS_MUTEX */

				if (!HasSystem<C>())
					shouldSkip = true;

			#ifdef HYPERECS_MUTEX
				systemLock.lock();
			#endif /* HYPERECS_MUTEX */
			};
			(lambda.template operator() < T > (), ...);
			return !shouldSkip;
		}

		/**
		 * @brief Getting all systems
		 *
		 * @return Returns all systems
		 */
		std::vector<System*> GetSystems()
		{
			std::vector<System*> systems;
			for (auto& system : m_Systems)
				systems.push_back(system.second);
			return systems;
		}

		/**
		 * @brief Calling from every system the OnTick function
		 *
		 * @param currentTick The current tick that is executing
		 */
		void OnTick(int currentTick)
		{
			for (auto& system : m_Systems)
				system.second->OnTick(m_Registry, currentTick);
		}

		/**
		 * @brief Calling from every system the OnUpdate function
		 *
		 * @param deltaTime The time difference between last update and current update
		 */
		void OnUpdate(Timestep deltaTime)
		{
			for (auto& system : m_Systems)
				system.second->OnUpdate(m_Registry, deltaTime);
		}

		/**
		 * @brief Calling from every system the OnRender function
		 */
		void OnRender()
		{
			for (auto& system : m_Systems)
				system.second->OnRender(m_Registry);
		}

		/**
		 * @brief Calling from every system the OnEvent function
		 */
		void OnEvent(Event& event)
		{
			for (auto& system : m_Systems)
				system.second->OnEvent(m_Registry, event);
		}
	};
}
