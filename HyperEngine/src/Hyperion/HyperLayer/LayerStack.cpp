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
		m_Layers.erase(std::remove(m_Layers.begin(), m_Layers.end(), layer), m_Layers.end());
	}

	void LayerStack::PopLayer(OverlayLayer* overlayLayer)
	{
		m_OverlayLayers.erase(std::remove(m_OverlayLayers.begin(), m_OverlayLayers.end(), overlayLayer), m_OverlayLayers.end());
	}

	void LayerStack::PopLayer(const std::string& layerName)
	{
		m_Layers.erase(std::remove_if(m_Layers.begin(), m_Layers.end(), [&](Layer* layer)
			{
				return layer->GetName() == layerName;
			}), m_Layers.end());
	}

	void LayerStack::PopOverlayLayer(const std::string& overlayLayerName)
	{
		m_OverlayLayers.erase(std::remove_if(m_OverlayLayers.begin(), m_OverlayLayers.end(), [&](OverlayLayer* overlayLayer)
			{
				return overlayLayer->GetName() == overlayLayerName;
			}), m_OverlayLayers.end());
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
		auto elementPosition = std::find_if(m_Layers.begin(), m_Layers.end(), [&](Layer* layer)
			{
				return layer->GetName() == layerName;
			});
		return elementPosition != m_Layers.end() ? *elementPosition : nullptr;
	}

	const OverlayLayer* LayerStack::GetOverlayLayer(const std::string& overlayLayerName) const
	{
		auto elementPosition = std::find_if(m_OverlayLayers.begin(), m_OverlayLayers.end(), [&](OverlayLayer* overlayLayer)
			{
				return overlayLayer->GetName() == overlayLayerName;
			});
		return elementPosition != m_OverlayLayers.end() ? *elementPosition : nullptr;
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
