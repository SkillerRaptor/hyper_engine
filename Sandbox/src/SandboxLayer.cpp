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
	m_SpriteShader = m_RenderContext->GetShaderManager()->CreateShader("assets/shaders/SpriteShaderVertex.glsl", "assets/shaders/SpriteShaderFragment.glsl");

	for (size_t i = 0; i < 100; i++)
	{
		HyperEntity square = m_Scene->CreateEntity(std::string("Square-" + i));
		square.AddComponent<SpriteRendererComponent>(glm::vec4(Random::Float(0.0f, 1.0f), Random::Float(0.0f, 1.0f), Random::Float(0.0f, 1.0f), 1.0f));
		auto& transform = square.GetComponent<TransformComponent>();
		static constexpr int32_t RANGE = 5;
		transform.Position += glm::vec3{ 1.0f * Random::Int16(-RANGE, RANGE), 1.0f * Random::Int16(-RANGE, RANGE), 1.0f * Random::Int16(-RANGE, RANGE) };
	}
}

void SandboxLayer::OnDetach()
{
}

void SandboxLayer::OnEvent(Event& event)
{
}

void SandboxLayer::OnUpdate(Timestep timeStep)
{
}

void SandboxLayer::OnRender()
{
}
