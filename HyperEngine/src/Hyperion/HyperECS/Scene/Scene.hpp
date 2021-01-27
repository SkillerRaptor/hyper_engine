#pragma once

#include <string>

#include "HyperCore/Core.hpp"
#include "HyperECS/HyperECS.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class HyperEntity;

	class Scene
	{
	private:
		std::string m_Name;

		Ref<Renderer2D> m_Renderer2D;
		Ref<World> m_World;

	public:
		Scene(const std::string& name, Ref<Renderer2D> renderer2D);
		~Scene();
		
		void Init();

		HyperEntity CreateEntity(const std::string& name = std::string());
		void DeleteEntity(HyperEntity& entity);

		void OnRender();
		void OnUpdate(Timestep timeStep);
		void OnEvent(Event& event);

		void Each(const typename std::common_type<std::function<void(HyperEntity)>>::type function);

		void SetName(const std::string& name);
		std::string GetName() const;

		World& GetWorld();
	};
}
