#pragma once

#include <typeinfo>
#include <unordered_map>

#include "EntitySystem.hpp"
#include "HyperCore/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperUtilities/Hasher.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class Registry;

	class EntitySystems
	{
	private:
		Registry& m_Registry;
		Ref<Renderer2D> m_Renderer2D;
		std::unordered_map<uint32_t, EntitySystem*> m_Systems;

	public:
		EntitySystems(Registry& registry, Ref<Renderer2D> renderer2D);

		void OnTick(int currentTick);
		void OnRender();
		void OnUpdate(Timestep timeStep);

		void OnEvent(Event& event);

		template <typename T, typename = typename std::enable_if<std::is_base_of<EntitySystem, T>::value, T>::type>
		void AddSystem()
		{
			HP_CORE_ASSERT(!HasSystem<T>(), "System is already registered!");
			T* system = new T();
			static_cast<EntitySystem*>(system)->SetRenderer2D(m_Renderer2D);
			m_Systems.emplace(Hasher::PrimeHasher(typeid(T).name()), std::move(system));
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
