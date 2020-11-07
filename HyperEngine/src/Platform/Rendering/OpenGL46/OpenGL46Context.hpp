#pragma once

#include "HyperRendering/RenderContext.hpp"

namespace Hyperion
{
	class OpenGL46Context : public RenderContext
	{
	public:
		OpenGL46Context();

		virtual void PreInit() override;
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;
	};
}
