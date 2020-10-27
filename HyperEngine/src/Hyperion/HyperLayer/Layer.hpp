#pragma once

#include <string>

#include "HyperEvents/HyperEvents.hpp"
#include "Rendering/RenderContext.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion 
{
	class Layer
	{
	protected:
		std::string m_LayerName;
		RenderContext* m_RenderContext;

	public:
		Layer(const std::string& name = "Default Layer");
		virtual ~Layer() = default;

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
