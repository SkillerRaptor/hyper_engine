#pragma once

#include <string>

#include "Registry.hpp"
#include "EntitySystems.hpp"
#include "Core/Core.hpp"
#include "Events/Event.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class Entity;

	class Scene
	{
	private:
		std::string m_Name;
		Ref<Registry> m_Registry;
		Ref<EntitySystems> m_Systems;

	public:
		Scene(const std::string& name);
		~Scene();
		
		void Init();

		Entity CreateEntity(const std::string& name = std::string());
		void DeleteEntity(Entity& entity);

		void OnTick(int currentTick);
		void OnRender();
		void OnUpdate(Timestep timeStep);

		void OnEvent(Event& event);

		void Each(const typename std::common_type<std::function<void(Entity)>>::type function);

		Registry& GetRegistry();

		void SetName(const std::string& name);
		std::string GetName() const;
	};
}
