#include "LayerStack.hpp"

namespace Hyperion 
{
	LayerStack::LayerStack()
	{
		m_ImGuiLayer = new ImGuiLayer();
		m_ImGuiLayer->OnAttach();
	}

	LayerStack::~LayerStack()
	{
		m_ImGuiLayer->OnDetach();
		delete m_ImGuiLayer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		layer->OnAttach();
		m_Layers.push_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto elementPosition = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (elementPosition != m_Layers.end())
			m_Layers.erase(elementPosition);
	}

	void LayerStack::PopLayer(std::string layerName)
	{
		for (Layer* layer : m_Layers)
			if (layer->GetName() == layerName)
				m_Layers.erase(std::find(m_Layers.begin(), m_Layers.end(), layer));
	}

	void LayerStack::PopLayer()
	{
		m_Layers.pop_back();
	}

	ImGuiLayer* LayerStack::GetImGuiLayer() const
	{
		return m_ImGuiLayer;
	}

	const std::vector<Layer*> LayerStack::GetLayers() const
	{
		return m_Layers;
	}
}