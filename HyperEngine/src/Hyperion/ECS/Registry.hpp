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

		uint32_t ConstructEntity();
		void DeleteEntity(uint32_t entity);

		template<class T, typename... Args>
		T& AddComponent(uint32_t entity, Args... args)
		{
			HP_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");

			T cls = { std::forward<Args>(args)... };
			return cls;
		}

		template<class T>
		T& RemoveComponent(uint32_t entity)
		{
			HP_CORE_ASSERT(!HasComponent<T>(), "Entity does not has component!");
		}

		template<class T>
		T& GetComponent(uint32_t entity)
		{
			HP_CORE_ASSERT(!HasComponent<T>(), "Entity does not has component!");
		}

		template<class T>
		bool HasComponent(uint32_t entity)
		{
			std::string = typeid(T).name();
		}

		template<class... Components>
		void Each(const typename std::common_type<std::function<void(Components&...)>>::type function)
		{
		}
	};
}