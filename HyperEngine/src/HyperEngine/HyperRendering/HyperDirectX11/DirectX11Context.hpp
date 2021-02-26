#pragma once

#ifdef HP_PLATFORM_WINDOWS

#include "HyperRendering/RenderContext.hpp"

namespace HyperRendering
{
	class DirectX11Context : public RenderContext
	{
	public:
		DirectX11Context(GraphicsAPI graphicsAPI);

		virtual void PreInit() override;
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnResize(size_t width, size_t height) override;

		virtual void OnPreUpdate() override;
		virtual void OnUpdate(HyperUtilities::Timestep timeStep) override;
		virtual void OnRender() override;
	};
}

#endif
