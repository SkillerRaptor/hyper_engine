#pragma once

#include "HyperRendering/RenderContext.hpp"

namespace Hyperion
{
	class DirectX11Context : public RenderContext
	{
	public:
		DirectX11Context();

		virtual void PreInit() override;
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnPreUpdate() override;
		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;
	};
}
