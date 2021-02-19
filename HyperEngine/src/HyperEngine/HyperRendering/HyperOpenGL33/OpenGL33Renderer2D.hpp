#pragma once

#include <glm/glm.hpp>

#include "HyperCore/Core.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperRendering/Vertex.hpp"
#include "HyperRendering/HyperOpenGL33/HyperBuffers/OpenGL33IndexBuffer.hpp"
#include "HyperRendering/HyperOpenGL33/HyperBuffers/OpenGL33VertexArray.hpp"
#include "HyperRendering/HyperOpenGL33/HyperBuffers/OpenGL33VertexBuffer.hpp"

namespace HyperRendering
{
	class OpenGL33Renderer2D : public Renderer2D
	{
	private:
		HyperCore::Ref<OpenGL33VertexArray> m_QuadVertexArray;
		HyperCore::Ref<OpenGL33VertexBuffer> m_QuadVertexBuffer;
		HyperCore::Ref<OpenGL33IndexBuffer> m_QuadIndexBuffer;
		uint32_t m_QuadShader{ 0 };

		Vertex2D* m_QuadVertexBufferBase;
		Vertex2D* m_QuadVertexBufferPtr;

	public:
		OpenGL33Renderer2D();
		~OpenGL33Renderer2D();

		virtual void Setup() override;

		virtual void BeginScene() override;
		virtual void EndScene() override;

		virtual void Flush() override;

		virtual void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color) override;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, TextureHandle texture) override;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color, TextureHandle texture) override;

		virtual void SetCamera(const glm::vec3& position, const glm::vec3& rotation, float fov, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, HyperECS::CameraComponent::ProjectionType projectionType) override;
		virtual void SetCamera(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) override;

	protected:
		virtual void StartBatch() override;
		virtual void NextBatch() override;
	};
}