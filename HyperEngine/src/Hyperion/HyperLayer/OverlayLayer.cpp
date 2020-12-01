#include "OverlayLayer.hpp"

namespace Hyperion 
{
	OverlayLayer::OverlayLayer(const std::string& name)
		: m_Name(name)
	{
	}

	void OverlayLayer::SetName(const std::string& name)
	{
		m_Name = name;
	}

	const std::string& OverlayLayer::GetName() const
	{
		return m_Name;
	}
}