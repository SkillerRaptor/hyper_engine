#pragma once

#include <string>

#include "HyperEvents/Event.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion 
{
	class OverlayLayer
	{
	protected:
		std::string m_LayerName;
		RenderContext* m_RenderContext;

	public:
		OverlayLayer(const std::string& name = "Default OverlayLayer");
		virtual ~OverlayLayer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(Event& event) { }
		virtual void OnTick(int currentTick) {}
		virtual void OnUpdate(Timestep timeStep) {}
		virtual void OnRender() {}

		void SetRenderContext(RenderContext* renderContext);
		const RenderContext* GetRenderContext() const;

		void SetName(const std::string& layerName);
		const std::string& GetName() const;
	};
}
