#include "OverlayLayer.hpp"

namespace Hyperion 
{
	OverlayLayer::OverlayLayer(const std::string& name)
		: m_LayerName(name)
	{
	}

	void OverlayLayer::SetRenderContext(RenderContext* renderContext)
	{
		m_RenderContext = renderContext;
	}

	const RenderContext* OverlayLayer::GetRenderContext() const
	{
		return m_RenderContext;
	}

	void OverlayLayer::SetName(const std::string& layerName)
	{
		m_LayerName = layerName;
	}

	const std::string& OverlayLayer::GetName() const
	{
		return m_LayerName;
	}
}
