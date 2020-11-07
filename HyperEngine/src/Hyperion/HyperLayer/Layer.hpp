#pragma once

#include <string>

#include "HyperCore/Core.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion 
{
	class Layer
	{
	protected:
		std::string m_LayerName;
		Ref<RenderContext> m_RenderContext;

	public:
		Layer(const std::string& name = "Default Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(Event& event) { }
		virtual void OnTick(int currentTick) {}
		virtual void OnUpdate(Timestep timeStep) {}
		virtual void OnRender() {}

		void SetRenderContext(Ref<RenderContext> renderContext);
		const Ref<RenderContext> GetRenderContext() const;

		void SetName(const std::string& layerName);
		const std::string& GetName() const;
	};
}
