#pragma once

#include "HyperRendering/RenderContext.hpp"

namespace HyperRendering
{
	class OpenGL46Context : public RenderContext
	{
	public:
		OpenGL46Context(GraphicsAPI graphicsAPI);

		virtual void PreInit() override;
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnResize(size_t width, size_t height) override;

		virtual void OnPreUpdate() override;
		virtual void OnUpdate(HyperUtilities::Timestep timeStep) override;
		virtual void OnRender() override;
	};
}
