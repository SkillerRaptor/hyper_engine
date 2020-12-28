#pragma once

#include <Hyperion.hpp>

class SandboxLayer : public Hyperion::Layer
{
private:
	Hyperion::ShaderHandle m_SpriteShader;

public:
	SandboxLayer();
	virtual ~SandboxLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnEvent(Hyperion::Event& event) override;
	virtual void OnUpdate(Hyperion::Timestep timeStep) override;
	virtual void OnRender() override;
};
