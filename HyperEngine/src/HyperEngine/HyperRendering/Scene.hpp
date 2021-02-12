#pragma once

#include <string>

#include "HyperCore/Core.hpp"
#include "HyperECS/Registry.hpp"
#include "HyperECS/HyperSystems/System.hpp"
#include "HyperEvent/Event.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace HyperRendering
{
	class Scene
	{
	private:
		std::string m_Name;
		HyperCore::Ref<Renderer2D> m_Renderer2D;

		HyperECS::Registry* m_Registry;
		std::vector<HyperECS::System*> m_Systems;

	public:
		Scene(const std::string& name, HyperCore::Ref<Renderer2D> renderer2D);
		~Scene();
		
		void Init();
		void Clear();

		void OnRender();
		void OnUpdate(HyperUtilities::Timestep timeStep);

		HyperECS::Entity CreateEntity(const std::string& name);
		void DestroyEntity(HyperECS::Entity entity);

		inline void SetName(const std::string& name)
		{
			m_Name = name;
		}

		inline const std::string& GetName() const
		{
			return m_Name;
		}

		inline HyperECS::Registry& GetRegistry()
		{
			return *m_Registry;
		}
	};
}
