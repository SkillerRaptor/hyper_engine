#include "Renderer2D.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	ShaderManager* Renderer2D::m_ShaderManager;
	TextureManager* Renderer2D::m_TextureManager;

	const uint32_t Renderer2D::MaxQuads = 20000;
	const uint32_t Renderer2D::MaxVertices = MaxQuads * 4;
	const uint32_t Renderer2D::MaxIndices = MaxQuads * 6;

	Vec4 Renderer2D::m_QuadVertexPositions[4];

	Ref<VertexArray> Renderer2D::m_QuadVertexArray;
	Ref<VertexBuffer> Renderer2D::m_QuadVertexBuffer;
	Ref<IndexBuffer> Renderer2D::m_QuadIndexBuffer;
	uint32_t Renderer2D::m_QuadShader;

	Vertex2D* Renderer2D::m_QuadVertexBufferBase;
	Vertex2D* Renderer2D::m_QuadVertexBufferPtr;

	uint32_t Renderer2D::m_QuadCount;
	uint32_t Renderer2D::m_QuadIndexCount;
	uint32_t Renderer2D::m_DrawCalls;

	void Renderer2D::Init()
	{
		m_QuadVertexArray = CreateRef<VertexArray>(VertexLayout::Vertex2D);
		m_QuadVertexArray->Bind();

		m_QuadVertexBuffer = CreateRef<VertexBuffer>(VertexLayout::Vertex2D, MaxVertices);
		m_QuadVertexBuffer->Bind();

		m_QuadVertexBufferBase = new Vertex2D[MaxVertices];

		uint32_t* quadIndices = new uint32_t[MaxIndices];
		uint32_t offset = 0;

		for (uint32_t i = 0; i < MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		m_QuadIndexBuffer = CreateRef<IndexBuffer>(quadIndices, MaxIndices);
		m_QuadIndexBuffer->Bind();
		delete[] quadIndices;

		m_QuadVertexPositions[0] = {  0.5f,  0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[2] = { -0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		m_QuadVertexArray->Init();
	}

	void Renderer2D::BeginScene(uint32_t width, uint32_t height, float zoom, float nearPlane, float farPlane, const Vec3& position)
	{
		float aspectRatio = (float) width / height;

		m_ShaderManager->UseShader(0);
		m_ShaderManager->SetMatrix4(0, "u_ProjectionMatrix", Matrix::Ortho(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom, nearPlane, farPlane));
		m_ShaderManager->SetMatrix4(0, "u_TransformationMatrix", Matrix::Translate(Matrix4<>(1.0f), position));

		m_QuadCount = 0;
		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase;
	}

	void Renderer2D::EndScene()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
		m_QuadVertexBuffer->Bind();
		m_QuadVertexBuffer->SetData(m_QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		if (m_QuadIndexCount == 0)
			return;

		m_ShaderManager->UseShader(0);

		m_QuadVertexArray->Bind();
		glDrawElements(GL_TRIANGLES, (unsigned int) m_QuadIndexCount, GL_UNSIGNED_INT, nullptr);

		m_DrawCalls++;
	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();

		m_QuadCount = 0;
		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase;
	}

	void Renderer2D::DrawQuad(const Vec3& position, const Vec3& rotation, const Vec3& scale, const Vec4& color)
	{
		constexpr size_t quadVertexCount = 4;
		const float tilingFactor = 1.0f;

		if (m_QuadIndexCount >= MaxIndices)
			FlushAndReset();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			m_QuadVertexBufferPtr->Position = Vec3(position.x + m_QuadVertexPositions[i].x, position.y + m_QuadVertexPositions[i].y, position.z + m_QuadVertexPositions[i].z);
			m_QuadVertexBufferPtr->Color = color;
			m_QuadVertexBufferPtr->TexCoords = {};
			m_QuadVertexBufferPtr->TexId = -1;
			m_QuadVertexBufferPtr++;
		}
		
		m_QuadIndexCount += 6;
		m_QuadCount++;
	}

	void Renderer2D::SetShaderManager(ShaderManager* shaderManager)
	{
		m_ShaderManager = shaderManager;
	}

	void Renderer2D::SetTextureManager(TextureManager* textureManager)
	{
		m_TextureManager = textureManager;
	}
}
