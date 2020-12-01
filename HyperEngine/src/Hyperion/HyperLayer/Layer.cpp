#include "Layer.hpp"

namespace Hyperion 
{
	Layer::Layer(const std::string& name)
		: m_Name(name)
	{
	}

	void Layer::SetName(const std::string& name)
	{
		m_Name = name;
	}

	const std::string& Layer::GetName() const
	{
		return m_LayerName;
	}
}
