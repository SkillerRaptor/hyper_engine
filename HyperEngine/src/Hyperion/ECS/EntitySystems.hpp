#pragma once

#include <typeinfo>
#include <unordered_map>

#include "EntitySystem.hpp"
#include "Core/Core.hpp"
#include "Events/Event.hpp"
#include "Utilities/Hasher.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class Registry;

	class EntitySystems
	{
	private:
		Registry& m_Registry;
		std::unordered_map<uint32_t, EntitySystem*> m_Systems;

	public:
		EntitySystems(Registry& registry);

		void OnTick(int currentTick);
		void OnRender();
		void OnUpdate(Timestep timeStep);

		void OnEvent(Event& event);

		template <typename T, typename = typename std::enable_if<std::is_base_of<EntitySystem, T>::value, T>::type>
		void AddSystem()
		{
			HP_CORE_ASSERT(!HasSystem<T>(), "System is already registered!");
			m_Systems.emplace(Hasher::PrimeHasher(typeid(T).name()), std::move(new T()));
		}

		template <typename T, typename = typename std::enable_if<std::is_base_of<EntitySystem, T>::value, T>::type>
		T& GetSystem()
		{
			HP_CORE_ASSERT(HasSystem<T>(), "System is not registered!");
			return m_Systems[Hasher::PrimeHasher(typeid(T).name())];
		}

		template <typename T, typename = typename std::enable_if<std::is_base_of<EntitySystem, T>::value, T>::type>
		void RemoveSystem()
		{
			HP_CORE_ASSERT(HasSystem<T>(), "System is not registered!");
			m_Systems.erase(Hasher::PrimeHasher(typeid(T).name()));
		}

		template <typename T, typename = typename std::enable_if<std::is_base_of<EntitySystem, T>::value, T>::type>
		bool HasSystem()
		{
			if (m_Systems.find(Hasher::PrimeHasher(typeid(T).name())) == m_Systems.end())
				return false;
			return true;
		}
	};
}
