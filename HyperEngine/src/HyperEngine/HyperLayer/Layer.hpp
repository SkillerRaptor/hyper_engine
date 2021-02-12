#pragma once

#include <string>

#include "HyperCore/Core.hpp"
#include "HyperEvent/Event.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperRendering/Scene.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace HyperLayer 
{
	class Layer
	{
	protected:
		std::string m_Name;
		HyperCore::Ref<HyperRendering::RenderContext> m_RenderContext;
		HyperCore::Ref<HyperRendering::Scene> m_Scene;

	public:
		Layer(const std::string& name)
			: m_Name{ name }, m_RenderContext{ nullptr }, m_Scene{ nullptr }
		{
		}

		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(HyperEvent::Event& event) {}

		virtual void OnUpdate(HyperUtilities::Timestep timeStep) {}
		virtual void OnRender() {}

		inline void SetName(const std::string& name)
		{
			m_Name = name;
		}

		inline const std::string& GetName() const
		{
			return m_Name;
		}

		inline void SetRenderContext(const HyperCore::Ref<HyperRendering::RenderContext>& renderContext)
		{
			m_RenderContext = renderContext;
		}

		inline const HyperCore::Ref<HyperRendering::RenderContext>& GetRenderContext()
		{
			return m_RenderContext;
		}

		inline void SetScene(const HyperCore::Ref<HyperRendering::Scene>& scene)
		{
			m_Scene = scene;
		}

		inline const HyperCore::Ref<HyperRendering::Scene>& GetScene()
		{
			return m_Scene;
		}
	};
}
