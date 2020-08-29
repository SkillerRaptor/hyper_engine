#include "LayerStack.h"

namespace Hyperion 
{
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
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
}