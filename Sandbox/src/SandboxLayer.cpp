#include "SandboxLayer.hpp"

SandboxLayer::SandboxLayer()
	: Layer{ "Sandbox Layer" }
{
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnAttach()
{
	m_SpriteShader = m_RenderContext->GetShaderManager()->CreateShader("assets/shaders/SpriteShaderVertex.glsl", "assets/shaders/SpriteShaderFragment.glsl");
}

void SandboxLayer::OnDetach()
{
}

void SandboxLayer::OnUpdate(HyperUtilities::Timestep timeStep)
{
}

void SandboxLayer::OnRender()
{
}

void SandboxLayer::RegisterEvents(HyperEvent::EventManager& eventManager)
{
}
