#pragma once

#include <string>

#include "HyperCore/Core.hpp"
#include "HyperECS/Registry.hpp"
#include "HyperECS/EntitySystems.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class Entity;

	class Scene
	{
	private:
		std::string m_Name;
		Renderer2D* m_Renderer2D;
		Ref<Registry> m_Registry;
		Ref<EntitySystems> m_Systems;

	public:
		Scene(const std::string& name, Renderer2D* renderer2D);
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
