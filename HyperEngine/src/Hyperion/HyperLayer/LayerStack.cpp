#include "LayerStack.hpp"

#include <algorithm>

namespace Hyperion
{
	LayerStack::LayerStack()
		: m_Layers({}), m_OverlayLayers({})
	{
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}

		for (OverlayLayer* overlayLayer : m_OverlayLayers)
		{
			overlayLayer->OnDetach();
			delete overlayLayer;
		}

		m_Layers.clear();
		m_OverlayLayers.clear();
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		layer->OnAttach();
		m_Layers.push_back(layer);
	}

	void LayerStack::PushLayer(OverlayLayer* overlayLayer)
	{
		overlayLayer->OnAttach();
		m_OverlayLayers.push_back(overlayLayer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto elementPosition = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (elementPosition != m_Layers.end())
			m_Layers.erase(elementPosition);
	}

	void LayerStack::PopLayer(OverlayLayer* overlayLayer)
	{
		auto elementPosition = std::find(m_OverlayLayers.begin(), m_OverlayLayers.end(), overlayLayer);
		if (elementPosition != m_OverlayLayers.end())
			m_OverlayLayers.erase(elementPosition);
	}

	void LayerStack::PopLayer(const std::string& layerName)
	{
		for (Layer* layer : m_Layers)
			if (layer->GetName() == layerName)
				m_Layers.erase(std::find(m_Layers.begin(), m_Layers.end(), layer));
	}

	void LayerStack::PopOverlayLayer(const std::string& overlayLayerName)
	{
		for (OverlayLayer* overlayLayer : m_OverlayLayers)
			if (overlayLayer->GetName() == overlayLayerName)
				m_OverlayLayers.erase(std::find(m_OverlayLayers.begin(), m_OverlayLayers.end(), overlayLayer));
	}

	void LayerStack::PopLayer()
	{
		m_Layers.pop_back();
	}

	void LayerStack::PopOverlayLayer()
	{
		m_OverlayLayers.pop_back();
	}

	Layer* LayerStack::GetLayer(const std::string& layerName)
	{
		for (Layer* layer : m_Layers)
			if (layer->GetName() == layerName)
				return layer;
		return nullptr;
	}

	OverlayLayer* LayerStack::GetOverlayLayer(const std::string& overlayLayerName)
	{
		for (OverlayLayer* overlayLayer : m_OverlayLayers)
			if (overlayLayer->GetName() == overlayLayerName)
				return overlayLayer;
		return nullptr;
	}

	const std::vector<Layer*>& LayerStack::GetLayers() const
	{
		return m_Layers;
	}

	const std::vector<OverlayLayer*>& LayerStack::GetOverlayLayers() const
	{
		return m_OverlayLayers;
	}
}
