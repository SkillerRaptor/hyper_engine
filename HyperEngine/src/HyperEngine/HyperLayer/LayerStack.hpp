#pragma once

#include <string>
#include <vector>

#include "HyperLayer/Layer.hpp"
#include "HyperLayer/OverlayLayer.hpp"

namespace HyperLayer
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
		void PushLayer(OverlayLayer* overlayLayer);

		void PopLayer(Layer* layer);
		void PopLayer(OverlayLayer* overlayLayer);

		void PopLayer(const std::string& layerName);
		void PopOverlayLayer(const std::string& layerName);

		void PopLastLayer();
		void PopLastOverlayLayer();

		const Layer* GetLayer(const std::string& layerName) const;
		const OverlayLayer* GetOverlayLayer(const std::string& layerName) const;

		inline const std::vector<Layer*>& GetLayers() const
		{
			return m_Layers;
		}

		inline const std::vector<OverlayLayer*>& GetOverlayLayers() const
		{
			return m_OverlayLayers;
		}
	};
}

