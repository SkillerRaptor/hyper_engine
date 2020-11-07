#include "Layer.hpp"

namespace Hyperion 
{
	Layer::Layer(const std::string& name)
		: m_LayerName(name)
	{
	}

	void Layer::SetName(const std::string& layerName)
	{
		m_LayerName = layerName;
	}

	const std::string& Layer::GetName() const
	{
		return m_LayerName;
	}
}
