#pragma once

#include <string>
#include <vector>

#include "Layer.hpp"
#include "OverlayLayer.hpp"

#include "Platform/Rendering/ImGui/ImGuiLayer.hpp"

namespace Hyperion 
{
	class LayerStack
	{
	private:
		std::vector<Layer*> m_Layers;
		std::vector<OverlayLayer*> m_OverlayLayers;

	public:
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PopLayer(const std::string& layerName);
		void PopLayer();
		Layer* GetLayer(const std::string& layerName);

		void PushOverlayLayer(OverlayLayer* overlayLayer);
		void PopOverlayLayer(OverlayLayer* overlayLayer);
		void PopOverlayLayer(const std::string& layerName);
		void PopOverlayLayer();
		OverlayLayer* GetOverlayLayer(const std::string& layerName);

		const std::vector<Layer*>& GetLayers() const;
		const std::vector<OverlayLayer*>& GetOverlayLayers() const;
	};
}

