#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Buffers/OpenGL46IndexBuffer.hpp"
#include "Buffers/OpenGL46VertexArray.hpp"
#include "Buffers/OpenGL46VertexBuffer.hpp"
#include "HyperCore/Core.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperRendering/Vertex.hpp"

namespace Hyperion
{
	class OpenGL46Renderer2D : public Renderer2D
	{
	private:
		Ref<OpenGL46VertexArray> m_QuadVertexArray;
		Ref<OpenGL46VertexBuffer> m_QuadVertexBuffer;
		Ref<OpenGL46IndexBuffer> m_QuadIndexBuffer;
		uint32_t m_QuadShader = { 0 };

		Vertex2D* m_QuadVertexBufferBase;
		Vertex2D* m_QuadVertexBufferPtr;

	public:
		OpenGL46Renderer2D();
		~OpenGL46Renderer2D();

		virtual void Setup() override;

		virtual void BeginScene() override;
		virtual void EndScene() override;

		virtual void Flush() override;

		virtual void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color) override;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, TextureHandle texture) override;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color, TextureHandle texture) override;

		virtual void SetCamera(const glm::vec3& position, const glm::vec3& rotation, float fov, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, CameraComponent::ProjectionType projectionType) override;
		virtual void SetCamera(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) override;

	protected:
		virtual void StartBatch() override;
		virtual void NextBatch() override;
	};
}
