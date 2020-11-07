#pragma once

#include <string>

#include "HyperCore/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion 
{
	class OverlayLayer
	{
	protected:
		std::string m_LayerName;
		Ref<RenderContext> m_RenderContext;

		friend class Application;

	public:
		OverlayLayer(const std::string& name = "Default OverlayLayer");
		virtual ~OverlayLayer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(Event& event) {}
		virtual void OnUpdate(Timestep timeStep) {}
		virtual void OnRender() {}

		void SetName(const std::string& layerName);
		const std::string& GetName() const;
	};
}
