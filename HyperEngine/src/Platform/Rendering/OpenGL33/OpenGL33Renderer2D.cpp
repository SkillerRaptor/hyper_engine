#include "OpenGL33Renderer2D.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	OpenGL33Renderer2D::OpenGL33Renderer2D()
	{
		m_QuadVertexArray = CreateRef<OpenGL33VertexArray>(VertexLayout::Vertex2D);
		m_QuadVertexArray->Bind();

		m_QuadVertexBuffer = CreateRef<OpenGL33VertexBuffer>(VertexLayout::Vertex2D, MaxVertices);
		m_QuadVertexBuffer->Bind();

		m_QuadVertexBufferBase = new Vertex2D[MaxVertices];
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase;

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

		m_QuadIndexBuffer = CreateRef<OpenGL33IndexBuffer>(quadIndices, MaxIndices);
		m_QuadIndexBuffer->Bind();
		delete[] quadIndices;

		m_QuadVertexPositions[0] = { 0.5f,  0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[2] = { -0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		m_QuadVertexArray->Init();
	}

	OpenGL33Renderer2D::~OpenGL33Renderer2D()
	{
		delete[] m_QuadVertexBufferBase;
	}

	void OpenGL33Renderer2D::BeginScene()
	{
		StartBatch();
	}

	void OpenGL33Renderer2D::EndScene()
	{
		Flush();
	}

	void OpenGL33Renderer2D::Flush()
	{
		if (m_QuadIndexCount == 0)
			return;

		uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
		m_QuadVertexBuffer->Bind();
		m_QuadVertexBuffer->SetData(m_QuadVertexBufferBase, dataSize / sizeof(Vertex2D));

		m_ShaderManager->UseShader({ 1 });

		m_QuadVertexArray->Bind();
		glDrawElements(GL_TRIANGLES, (unsigned int) m_QuadIndexCount, GL_UNSIGNED_INT, nullptr);

		m_DrawCalls++;
	}

	void OpenGL33Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color)
	{
		if (m_QuadIndexCount >= MaxIndices)
			NextBatch();

		constexpr size_t quadVertexCount = 4;

		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i];
			m_QuadVertexBufferPtr->Color = color;
			m_QuadVertexBufferPtr->TextureCoords = {};
			m_QuadVertexBufferPtr->TextureId = -1;
			m_QuadVertexBufferPtr++;
		}
		
		m_QuadIndexCount += 6;
		m_QuadCount++;
	}

	void OpenGL33Renderer2D::StartBatch()
	{
		m_QuadCount = 0;
		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase;
	}

	void OpenGL33Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void OpenGL33Renderer2D::SetCamera(const glm::vec3& position, const glm::vec3& rotation, float fov, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, CameraComponent::ProjectionType projectionType)
	{
		glm::mat4 projectionMatrix(1.0f);
		glm::mat4 viewMatrix(1.0f);

		float aspectRatio = 0.0f;
		TextureHandle gameColorTexture = m_SceneRecorder->GetGameColorAttachment();

		if (gameColorTexture.IsHandleValid())
		{
			TextureData* gameColorTextureData = m_TextureManager->GetTextureData(gameColorTexture);
			aspectRatio = (static_cast<float>(gameColorTextureData->Width) * viewportRect.x) / (static_cast<float>(gameColorTextureData->Height) * viewportRect.y);
		}

		switch (projectionType)
		{
		case CameraComponent::ProjectionType::ORTHOGRAPHIC:
			projectionMatrix = glm::ortho(-aspectRatio * fov, aspectRatio * fov, -fov, fov, clippingPlanes.x, clippingPlanes.y);
			viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.z, position.y, position.x));
			break;
		case CameraComponent::ProjectionType::PERSPECTIVE:
			projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, clippingPlanes.x, clippingPlanes.y);
			viewMatrix = glm::translate(viewMatrix, glm::vec3{ position.x, -position.y, position.z });
			viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), { 0.0f, 0.0f, 1.0f });
			viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), { 0.0f, 1.0f, 0.0f });
			viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), { 1.0f, 0.0f, 0.0f });
			break;
		default:
			break;
		}

		m_ShaderManager->UseShader({ 1 });
		m_ShaderManager->SetMatrix4({ 1 }, "u_ProjectionMatrix", projectionMatrix);
		m_ShaderManager->SetMatrix4({ 1 }, "u_ViewMatrix", viewMatrix);
	}

	void OpenGL33Renderer2D::SetCamera(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
	{
		m_ShaderManager->UseShader({ 1 });
		m_ShaderManager->SetMatrix4({ 1 }, "u_ProjectionMatrix", projectionMatrix);
		m_ShaderManager->SetMatrix4({ 1 }, "u_ViewMatrix", viewMatrix);
	}
}
