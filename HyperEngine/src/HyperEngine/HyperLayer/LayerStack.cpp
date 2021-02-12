#include "HyperLayer/LayerStack.hpp"

#include <algorithm>

namespace HyperLayer
{
	LayerStack::LayerStack()
		: m_Layers{}, m_OverlayLayers{}
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
		m_Layers.emplace_back(layer);
	}

	void LayerStack::PushLayer(OverlayLayer* overlayLayer)
	{
		overlayLayer->OnAttach();
		m_OverlayLayers.emplace_back(overlayLayer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		std::vector<Layer*>::iterator it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it == m_Layers.end())
			return;
		(*it)->OnDetach();
		m_Layers.erase(it);
	}

	void LayerStack::PopLayer(OverlayLayer* overlayLayer)
	{
		std::vector<OverlayLayer*>::iterator it = std::find(m_OverlayLayers.begin(), m_OverlayLayers.end(), overlayLayer);
		if (it == m_OverlayLayers.end())
			return;
		(*it)->OnDetach();
		m_OverlayLayers.erase(it);
	}

	void LayerStack::PopLayer(const std::string& layerName)
	{
		std::vector<Layer*>::iterator it = std::find_if(m_Layers.begin(), m_Layers.end(), [layerName](Layer* layer)
			{
				return layer->GetName() == layerName;
			});
		if (it == m_Layers.end())
			return;
		(*it)->OnDetach();
		m_Layers.erase(it);
	}

	void LayerStack::PopOverlayLayer(const std::string& overlayLayerName)
	{
		std::vector<OverlayLayer*>::iterator it = std::find_if(m_OverlayLayers.begin(), m_OverlayLayers.end(), [overlayLayerName](OverlayLayer* overlayLayer)
			{
				return overlayLayer->GetName() == overlayLayerName;
			});
		if (it == m_OverlayLayers.end())
			return;
		(*it)->OnDetach();
		m_OverlayLayers.erase(it);
	}

	void LayerStack::PopLastLayer()
	{
		m_Layers.pop_back();
	}

	void LayerStack::PopLastOverlayLayer()
	{
		m_OverlayLayers.pop_back();
	}

	const Layer* LayerStack::GetLayer(const std::string& layerName) const
	{
		std::vector<Layer*>::const_iterator elementPosition = std::find_if(m_Layers.begin(), m_Layers.end(), [layerName](Layer* layer)
			{
				return layer->GetName() == layerName;
			});
		return elementPosition != m_Layers.end() ? *elementPosition : nullptr;
	}

	const OverlayLayer* LayerStack::GetOverlayLayer(const std::string& overlayLayerName) const
	{
		std::vector<OverlayLayer*>::const_iterator elementPosition = std::find_if(m_OverlayLayers.begin(), m_OverlayLayers.end(), [overlayLayerName](OverlayLayer* overlayLayer)
			{
				return overlayLayer->GetName() == overlayLayerName;
			});
		return elementPosition != m_OverlayLayers.end() ? *elementPosition : nullptr;
	}
}
