#pragma once

#include "Core/Core.hpp"
#include "Rendering/Vertex.hpp"
#include "Maths/Matrix.hpp"
#include "Maths/Vector.hpp"

#include "Platform/OpenGL/OpenGLShaderManager.hpp"
#include "Platform/OpenGL/OpenGLTextureManager.hpp"
#include "Platform/OpenGL/Buffers/OpenGLIndexBuffer.hpp"
#include "Platform/OpenGL/Buffers/OpenGLVertexArray.hpp"
#include "Platform/OpenGL/Buffers/OpenGLVertexBuffer.hpp"

namespace Hyperion
{
	class OpenGLRenderer2D
	{
	private:
		static OpenGLShaderManager* m_ShaderManager;
		static OpenGLTextureManager* m_TextureManager;

		static const uint32_t MaxQuads;
		static const uint32_t MaxVertices;
		static const uint32_t MaxIndices;

		static Vec4 m_QuadVertexPositions[4];

		static Ref<OpenGLVertexArray> m_QuadVertexArray;
		static Ref<OpenGLVertexBuffer> m_QuadVertexBuffer;
		static Ref<OpenGLIndexBuffer> m_QuadIndexBuffer;
		static uint32_t m_QuadShader;

		static Vertex2D* m_QuadVertexBufferBase;
		static Vertex2D* m_QuadVertexBufferPtr;

		static uint32_t m_QuadCount;
		static uint32_t m_QuadIndexCount;
		static uint32_t m_DrawCalls;

	public:
		static void Init();

		static void BeginScene(uint32_t width, uint32_t height, float zoom, float nearPlane, float farPlane, const Vec3& position);
		static void EndScene();

		static void Flush();
		static void FlushAndReset();

		static void DrawQuad(const Vec3& position, const Vec3& rotation, const Vec3& scale, const Vec4& color);

		static void SetShaderManager(OpenGLShaderManager* shaderManager);
		static void SetTextureManager(OpenGLTextureManager* textureManager);
	};
}
