#pragma once

#include <HyperEngine.hpp>

class SandboxLayer : public HyperLayer::Layer
{
private:
	HyperRendering::ShaderHandle m_SpriteShader{ 0 };

public:
	SandboxLayer();
	virtual ~SandboxLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnEvent(HyperEvent::Event& event) override;
	virtual void OnUpdate(HyperUtilities::Timestep timeStep) override;
	virtual void OnRender() override;
};
