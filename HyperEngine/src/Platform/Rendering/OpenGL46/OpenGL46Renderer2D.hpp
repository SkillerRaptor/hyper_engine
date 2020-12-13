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
		uint32_t m_QuadShader;

		Vertex2D* m_QuadVertexBufferBase;
		Vertex2D* m_QuadVertexBufferPtr;

	public:
		OpenGL46Renderer2D();
		~OpenGL46Renderer2D();

		virtual void BeginScene() override;
		virtual void EndScene() override;

		virtual void Flush() override;

		virtual void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color) override;

		virtual void SetShaderManager(Ref<ShaderManager> shaderManager) override;
		virtual void SetTextureManager(Ref<TextureManager> textureManager) override;

	protected:
		virtual void StartBatch() override;
		virtual void NextBatch() override;
	};
}
