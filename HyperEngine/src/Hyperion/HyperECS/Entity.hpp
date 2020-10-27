#pragma once

#include <cstdint>

#include "Scene/Scene.hpp"

namespace Hyperion
{
	class Entity
	{
	private:
		uint32_t m_EntityHandle;
		Scene* m_Scene;

	public:
		Entity(const Entity& other) = default;
		Entity(uint32_t entityHandle, Scene* scene)
			: m_EntityHandle(entityHandle), m_Scene(scene) {}

		template<class T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Scene->GetRegistry().AddComponent<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<class T>
		void RemoveComponent()
		{
			m_Scene->GetRegistry().RemoveComponent<T>(m_EntityHandle);
		}

		template<class T>
		T& GetComponent()
		{
			return m_Scene->GetRegistry().GetComponent<T>(m_EntityHandle);
		}

		template<class T>
		bool HasComponent()
		{
			return m_Scene->GetRegistry().HasComponent<T>(m_EntityHandle);
		}

		uint32_t GetEntityHandle() const { return m_EntityHandle; }

		operator bool() const { return (uint32_t) m_EntityHandle != -1; }
		operator uint32_t() const { return m_EntityHandle; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	};
}
