#pragma once

#include <cstdint>

#include "Scene/Scene.hpp"

namespace Hyperion
{
	class HyperEntity
	{
	private:
		Entity m_EntityHandle;
		Scene* m_Scene;

	public:
		HyperEntity(const HyperEntity& other) = default;
		HyperEntity(Entity entityHandle, Scene* scene)
			: m_EntityHandle(entityHandle), m_Scene(scene) {}

		template<class T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Scene->GetWorld().AddComponent<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<class T>
		void RemoveComponent()
		{
			m_Scene->GetWorld().RemoveComponent<T>(m_EntityHandle);
		}

		template<class T>
		T& GetComponent()
		{
			return m_Scene->GetWorld().GetComponent<T>(m_EntityHandle);
		}

		template<class T>
		bool HasComponent()
		{
			return m_Scene->GetWorld().HasComponent<T>(m_EntityHandle);
		}

		Entity GetEntityHandle() const { return m_EntityHandle; }

		operator bool() const { return m_EntityHandle.IsHandleValid(); }
		operator size_t() const { return m_EntityHandle.Handle; }

		bool operator==(const HyperEntity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const HyperEntity& other) const
		{
			return !(*this == other);
		}
	};
}
