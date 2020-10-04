#include "SandboxLayer.hpp"

SandboxLayer::SandboxLayer()
	: Layer("Sandbox Layer")
{
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnAttach()
{
	//uint32_t shaderHandle = m_RenderContext->GetShaderManager()->CreateShader("", "");
}

void SandboxLayer::OnDetach()
{
}

void SandboxLayer::OnEvent(Hyperion::Event& event)
{
}

void SandboxLayer::OnTick(int currentTick)
{
}

void SandboxLayer::OnUpdate(Hyperion::Timestep timeStep)
{
}

void SandboxLayer::OnRender()
{
}