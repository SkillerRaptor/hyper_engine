#pragma once

#include "Core/Core.hpp"
#include "Rendering/Renderer2D.hpp"
#include "Rendering/Vertex.hpp"
#include "Maths/Matrix.hpp"
#include "Maths/Vector.hpp"

#include "Platform/OpenGL/Buffers/OpenGLIndexBuffer.hpp"
#include "Platform/OpenGL/Buffers/OpenGLVertexArray.hpp"
#include "Platform/OpenGL/Buffers/OpenGLVertexBuffer.hpp"

namespace Hyperion
{
	class OpenGLRenderer2D : public Renderer2D
	{
	private:
		Ref<OpenGLVertexArray> m_QuadVertexArray;
		Ref<OpenGLVertexBuffer> m_QuadVertexBuffer;
		Ref<OpenGLIndexBuffer> m_QuadIndexBuffer;
		uint32_t m_QuadShader;

		Vertex2D* m_QuadVertexBufferBase;
		Vertex2D* m_QuadVertexBufferPtr;

	public:
		OpenGLRenderer2D();
		~OpenGLRenderer2D();

		virtual void BeginScene(uint32_t width, uint32_t height, float zoom, float nearPlane, float farPlane, const Vec3& position) override;
		virtual void EndScene() override;

		virtual void Flush() override;
		virtual void FlushAndReset() override;

		virtual void DrawQuad(const Vec3& position, const Vec3& rotation, const Vec3& scale, const Vec4& color) override;

		virtual void SetShaderManager(ShaderManager* shaderManager) override;
		virtual void SetTextureManager(TextureManager* textureManager) override;
	};
}
