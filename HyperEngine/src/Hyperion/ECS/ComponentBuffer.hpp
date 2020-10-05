#pragma once

#include <unordered_map>

#include "Core/Core.hpp"
#include "Utilities/Hasher.hpp"

namespace Hyperion
{
	class ComponentBuffer
	{
	private:
		std::unordered_map<uint32_t, void*> m_Components;

	public:
		ComponentBuffer();
		~ComponentBuffer();

		template<class T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			HP_CORE_ASSERT(HasComponent<T>(), "Entity already has component!");
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			m_Components.emplace(componentId, new T(std::forward<Args>(args)...));
			return GetComponent<T>();
		}

		template<class T>
		void RemoveComponent()
		{
			HP_CORE_ASSERT(!HasComponent<T>(), "Entity does not has component!");
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			m_Components.erase(componentId);
		}

		template<class T>
		T& GetComponent()
		{
			HP_CORE_ASSERT(!HasComponent<T>(), "Entity does not has component!");
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			return *static_cast<T*>(m_Components[componentId]);
		}

		template<class T>
		bool HasComponent()
		{
			uint32_t componentId = Hasher::PrimeHasher(typeid(T).name());
			return m_Components.find(componentId) != m_Components.end();
		}
	};
}