#pragma once

#include "Buffers/OpenGL33IndexBuffer.hpp"
#include "Buffers/OpenGL33VertexArray.hpp"
#include "Buffers/OpenGL33VertexBuffer.hpp"
#include "HyperCore/Core.hpp"
#include "HyperMath/Vector.hpp"
#include "HyperRendering/Renderer2D.hpp"
#include "HyperRendering/Vertex.hpp"

namespace Hyperion
{
	class OpenGL33Renderer2D : public Renderer2D
	{
	private:
		Ref<OpenGL33VertexArray> m_QuadVertexArray;
		Ref<OpenGL33VertexBuffer> m_QuadVertexBuffer;
		Ref<OpenGL33IndexBuffer> m_QuadIndexBuffer;
		uint32_t m_QuadShader;

		Vertex2D* m_QuadVertexBufferBase;
		Vertex2D* m_QuadVertexBufferPtr;

	public:
		OpenGL33Renderer2D();
		~OpenGL33Renderer2D();

		virtual void BeginScene(CameraInfo cameraInfo) override;
		virtual void EndScene() override;

		virtual void Flush() override;

		virtual void DrawQuad(const Vec3& position, const Vec3& rotation, const Vec3& scale, const Vec4& color) override;

		virtual void SetShaderManager(Ref<ShaderManager> shaderManager) override;
		virtual void SetTextureManager(Ref<TextureManager> textureManager) override;

	protected:
		virtual void StartBatch() override;
		virtual void NextBatch() override;
	};
}
