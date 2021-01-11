#pragma once

#include <string>

#include "HyperCore/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperUtilities/Timestep.hpp"

#include "HyperECS/Scene/Scene.hpp"
#include "HyperRendering/SceneRecorder.hpp"

namespace Hyperion 
{
	class OverlayLayer
	{
	protected:
		std::string m_Name;
		Ref<RenderContext> m_RenderContext;

		Ref<Scene> m_Scene;
		Ref<SceneRecorder> m_SceneRecorder;

		friend class Application;

	public:
		OverlayLayer(const std::string& name);
		virtual ~OverlayLayer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(Event& event) {}

		virtual void OnUpdate(Timestep timeStep) {}
		virtual void OnRender() {}

		void SetName(const std::string& name);
		const std::string& GetName() const;
	};
}
