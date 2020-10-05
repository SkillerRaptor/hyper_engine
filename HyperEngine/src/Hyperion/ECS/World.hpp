#pragma once

#include <functional>
#include <type_traits>
#include <unordered_map>

#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class EntitySystem;

	class World
	{
	private:
		std::vector<EntitySystem*> m_EntitySystems;

	public:
		World();
		~World();

		void Tick(int currentTick);
		void Update(Timestep timeStep);
		void Render();

		uint32_t ConstructEntity();
		void DeleteEntity(uint32_t entity);

		template<class... Components>
		void Each(const typename std::common_type<std::function<void(Components&...)>>::type function)
		{
		}
	};
}