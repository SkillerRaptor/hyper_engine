#pragma once

#include <string>
#include <vector>

#include "Layer.hpp"
#include "ImGuiLayer.hpp"

namespace Hyperion 
{
	class LayerStack
	{
	private:
		std::vector<Layer*> m_Layers;
		ImGuiLayer* m_ImGuiLayer;

	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);

		void PopLayer(Layer* layer);
		void PopLayer(std::string layerName);
		void PopLayer();

		ImGuiLayer* GetImGuiLayer() const;
		const std::vector<Layer*> GetLayers() const;
	};
}

