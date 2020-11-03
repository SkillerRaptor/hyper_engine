#include "LayerStack.hpp"

namespace Hyperion 
{
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

	void LayerStack::PopLayer(const std::string& layerName)
	{
		for (Layer* layer : m_Layers)
			if (layer->GetName() == layerName)
				m_Layers.erase(std::find(m_Layers.begin(), m_Layers.end(), layer));
	}

	void LayerStack::PopLayer()
	{
		m_Layers.pop_back();
	}

	Layer* LayerStack::GetLayer(const std::string& layerName)
	{
		for (Layer* layer : m_Layers)
			if (layer->GetName() == layerName)
				return layer;
		return nullptr;
	}

	void LayerStack::PushOverlayLayer(OverlayLayer* overlayLayer)
	{
		overlayLayer->OnAttach();
		m_OverlayLayers.push_back(overlayLayer);
	}

	void LayerStack::PopOverlayLayer(OverlayLayer* overlayLayer)
	{
		auto elementPosition = std::find(m_OverlayLayers.begin(), m_OverlayLayers.end(), overlayLayer);
		if (elementPosition != m_OverlayLayers.end())
			m_OverlayLayers.erase(elementPosition);
	}

	void LayerStack::PopOverlayLayer(const std::string& layerName)
	{
		for (OverlayLayer* overlayLayer : m_OverlayLayers)
			if (overlayLayer->GetName() == layerName)
				m_OverlayLayers.erase(std::find(m_OverlayLayers.begin(), m_OverlayLayers.end(), overlayLayer));
	}

	void LayerStack::PopOverlayLayer()
	{
		m_OverlayLayers.pop_back();
	}

	OverlayLayer* LayerStack::GetOverlayLayer(const std::string& layerName)
	{
		for (OverlayLayer* overlayLayer : m_OverlayLayers)
			if (overlayLayer->GetName() == layerName)
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
