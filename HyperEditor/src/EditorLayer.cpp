#include "EditorLayer.hpp"

EditorLayer::EditorLayer()
	: OverlayLayer("Editor Layer")
{
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnAttach()
{
	m_SpriteShader = m_RenderContext->GetShaderManager()->CreateShader("assets/shaders/SpriteShaderVertex.glsl", "assets/shaders/SpriteShaderFragment.glsl");
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnEvent(Hyperion::Event& event)
{
}

void EditorLayer::OnUpdate(Hyperion::Timestep timeStep)
{
}

void EditorLayer::OnRender()
{
}
