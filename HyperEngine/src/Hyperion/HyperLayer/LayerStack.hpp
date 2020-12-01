#pragma once

#include <string>
#include <vector>

#include "Layer.hpp"
#include "OverlayLayer.hpp"

namespace Hyperion 
{
	class LayerStack
	{
	private:
		std::vector<Layer*> m_Layers;
		std::vector<OverlayLayer*> m_OverlayLayers;

	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlayLayer(OverlayLayer* overlayLayer);

		void PopLayer(Layer* layer);
		void PopOverlayLayer(OverlayLayer* overlayLayer);

		void PopLayer(const std::string& layerName);
		void PopOverlayLayer(const std::string& layerName);

		void PopLayer();
		void PopOverlayLayer();

		Layer* GetLayer(const std::string& layerName);
		OverlayLayer* GetOverlayLayer(const std::string& layerName);

		const std::vector<Layer*>& GetLayers() const;
		const std::vector<OverlayLayer*>& GetOverlayLayers() const;
	};
}
