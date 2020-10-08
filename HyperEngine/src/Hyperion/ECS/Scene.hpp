#pragma once

#include <string>

#include "Registry.hpp"
#include "EntitySystems.hpp"
#include "Core/Core.hpp"
#include "Events/Event.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class Scene
	{
	private:
		Ref<Registry> m_Registry;
		Ref<EntitySystems> m_Systems;

	public:
		Scene();
		~Scene();
		
		void Init();

		uint32_t CreateEntity(const std::string& name = std::string());

		void OnTick(int currentTick);
		void OnRender();
		void OnUpdate(Timestep timeStep);

		void OnEvent(Event& event);

		Registry& GetRegistry();
	};
}
