#pragma once

#include <functional>
#include <type_traits>
#include <unordered_map>

#include "Component.h"
#include "Entity.h"
#include "EntitySystem.h"

namespace Hyperion
{
	class World
	{
	private:
		std::vector<Entity> m_Entities;
		std::vector<EntitySystem*> m_EntitySystems;

	public:
		World();
		~World();

		Entity* ConstructEntity();
		void DeleteEntity(Entity* entity);

		template<class... Components>
		void Each(const typename std::common_type<std::function<void(Entity*, Components...)>>::type function)
		{
			for (Entity entity : m_Entities)
			{
				bool breakIteration = false;
				([]<typename T>() {
					if (breakIteration) return;
					if (!entity.HasComponent<T>())
						breakIteration = true;
				}.template operator() < Components > (), ...);
				function(&entity, entity.GetComponents<Components>()...);
			}
		}
	};
}