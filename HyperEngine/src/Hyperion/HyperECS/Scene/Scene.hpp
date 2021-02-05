#pragma once

#include <string>

#include "HyperCore/Core.hpp"
#include "HyperECS/Registry.hpp"
#include "HyperECS/Systems/System.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class Scene
	{
	private:
		std::string m_Name;
		Ref<Renderer2D> m_Renderer2D;

		std::vector<System*> m_Systems;

		Registry* m_Registry;

	public:
		Scene(const std::string& name, Ref<Renderer2D> renderer2D);
		~Scene();
		
		void Init();
		void Clear();

		void OnRender();
		void OnUpdate(Timestep timeStep);

		Entity CreateEntity(const std::string& name);
		void DestroyEntity(Entity entity);

		void SetName(const std::string& name);
		std::string GetName() const;

		Registry& GetRegistry();
	};
}
