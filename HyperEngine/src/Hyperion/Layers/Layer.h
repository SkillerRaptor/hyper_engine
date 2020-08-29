#pragma once

#include <string>

#include "Events/Event.h"
#include "Utilities/Timestep.h"

namespace Hyperion 
{
	class Layer
	{
	protected:
		std::string m_LayerName;

	public:
		Layer(const std::string& name = "Default Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(Event& event) { }
		virtual void OnTick(int currentTick) {}
		virtual void OnUpdate(Timestep timeStep) {}
		virtual void OnRender() {}

		const std::string& GetName() const { return m_LayerName; }
	};
}