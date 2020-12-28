#pragma once

#include <Hyperion.hpp>

class EditorLayer : public Hyperion::OverlayLayer
{
private:
	Hyperion::ShaderHandle m_SpriteShader;

public:
	EditorLayer();
	virtual ~EditorLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnEvent(Hyperion::Event& event) override;
	virtual void OnUpdate(Hyperion::Timestep timeStep) override;
	virtual void OnRender() override;
};
