#pragma once

#include <string>
#include <vector>

#include "Layer.h"

namespace Hyperion 
{
	class LayerStack
	{
	private:
		std::vector<Layer*> m_Layers;

	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);

		void PopLayer(Layer* layer);
		void PopLayer(std::string layerName);
		void PopLayer();

		const std::vector<Layer*> GetLayers() const { return m_Layers; }
	};
}

