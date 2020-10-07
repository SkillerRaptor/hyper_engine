#pragma once

#include <string>

#include "Registry.hpp"
#include "Systems/EntitySystems.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class Scene
	{
	private:
		Registry m_Registry;
		EntitySystems m_Systems;

	public:
		Scene();
		~Scene();
		
		void Init();

		uint32_t CreateEntity(const std::string& name = std::string());

		void OnTick(int currentTick);
		void OnRender();
		void OnUpdate(Timestep timeStep);

		Registry& GetRegistry();
	};
}
