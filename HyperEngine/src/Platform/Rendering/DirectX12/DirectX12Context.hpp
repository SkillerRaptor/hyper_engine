#pragma once

#include "HyperRendering/RenderContext.hpp"

namespace Hyperion
{
	class DirectX12Context : public RenderContext
	{
	public:
		DirectX12Context();

		virtual void PreInit() override;
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnTick(int currentTick) override;
		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;
	};
}
