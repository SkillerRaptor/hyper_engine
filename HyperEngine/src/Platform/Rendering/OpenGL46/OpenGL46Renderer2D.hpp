#pragma once

#include "Buffers/OpenGL46IndexBuffer.hpp"
#include "Buffers/OpenGL46VertexArray.hpp"
#include "Buffers/OpenGL46VertexBuffer.hpp"
#include "HyperCore/Core.hpp"
#include "HyperMath/Vector.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperRendering/Buffers/Vertex.hpp"

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

		virtual void BeginScene(uint32_t width, uint32_t height, float zoom, float nearPlane, float farPlane, const Vec3& position) override;
		virtual void EndScene() override;

		virtual void Flush() override;
		virtual void FlushAndReset() override;

		virtual void DrawQuad(const Vec3& position, const Vec3& rotation, const Vec3& scale, const Vec4& color) override;

		virtual void SetShaderManager(Ref<ShaderManager> shaderManager) override;
		virtual void SetTextureManager(Ref<TextureManager> textureManager) override;
	};
}
