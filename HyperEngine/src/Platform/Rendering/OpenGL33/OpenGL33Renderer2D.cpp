#include "OpenGL33Renderer2D.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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

		m_QuadVertexArray->Init();
	}

	OpenGL33Renderer2D::~OpenGL33Renderer2D()
	{
		delete[] m_QuadVertexBufferBase;
	}

	void OpenGL33Renderer2D::Setup()
	{
		int32_t samplers[32];
		for (uint32_t i = 0; i < MaxTextureSlots; i++)
			samplers[i] = i;

		m_ShaderManager->UseShader({ 1 });
		m_ShaderManager->SetIntegerArray({ 1 }, "u_Textures", MaxTextureSlots, samplers);

		uint32_t whiteTextureData = 0xFFFFFFFF;

		TextureHandle whiteTexture = m_TextureManager->CreateTexture(1, 1, TextureType::DEFAULT);
		m_TextureManager->SetTexturePixels(whiteTexture, &whiteTextureData, sizeof(uint32_t));

		m_TextureSlots.resize(MaxTextureSlots);
		for (size_t i = 0; i < MaxTextureSlots; i++)
			m_TextureSlots[i] = TextureHandle{ 0 };
		m_TextureSlots[0] = whiteTexture;
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

		for (uint32_t i = 0; i < m_TextureSlotIndex; i++)
			m_TextureManager->BindTexture(m_TextureSlots[i], i);

		m_QuadVertexArray->Bind();
		glDrawElements(GL_TRIANGLES, (unsigned int)m_QuadIndexCount, GL_UNSIGNED_INT, nullptr);

		m_DrawCalls++;
	}

	void OpenGL33Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color)
	{
		DrawQuad(position, rotation, scale, color, TextureHandle{ 0 });
	}

	void OpenGL33Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, TextureHandle texture)
	{
		DrawQuad(position, rotation, scale, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, texture);
	}

	void OpenGL33Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color, TextureHandle texture)
	{
		static constexpr const size_t quadVertexCount = 4;
		static constexpr const glm::vec3 vertexPositions[4] =
		{
			{  0.5f,  0.5f, 0.0f },
			{  0.5f, -0.5f, 0.0f },
			{ -0.5f, -0.5f, 0.0f },
			{ -0.5f,  0.5f, 0.0f }
		};

		static constexpr const glm::vec2 textureCoords[4] =
		{
			{ 1.0f, 1.0f },
			{ 1.0f, 0.0f },
			{ 0.0f, 0.0f },
			{ 0.0f, 1.0f }
		};

		if (m_QuadIndexCount >= MaxIndices)
			NextBatch();

		uint32_t textureIndex = 0;

		if (texture.IsHandleValid())
		{
			for (uint32_t i = 1; i < m_TextureSlotIndex; i++)
			{
				if (m_TextureSlots[i] == texture)
				{
					textureIndex = i;
					break;
				}
			}

			if (textureIndex == 0)
			{
				if (m_TextureSlotIndex >= MaxTextureSlots)
					NextBatch();

				textureIndex = m_TextureSlotIndex;
				m_TextureSlots[m_TextureSlotIndex] = texture;
				m_TextureSlotIndex++;
			}
		}

		glm::quat quaternion = glm::quat(glm::vec3{ glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z) });

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(quaternion) * glm::scale(glm::mat4(1.0f), scale);

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			m_QuadVertexBufferPtr->Position = transform * glm::vec4{ vertexPositions[i], 1.0f };
			m_QuadVertexBufferPtr->Color = color;
			m_QuadVertexBufferPtr->TextureCoords = textureCoords[i];
			m_QuadVertexBufferPtr->TextureId = textureIndex;
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
			uint32_t width = m_TextureManager->GetWidth(gameColorTexture).value_or(0);
			uint32_t height = m_TextureManager->GetHeight(gameColorTexture).value_or(0);
			aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		}

		switch (projectionType)
		{
		case CameraComponent::ProjectionType::ORTHOGRAPHIC:
			projectionMatrix = glm::ortho(-aspectRatio * fov, aspectRatio * fov, -fov, fov, clippingPlanes.x, clippingPlanes.y);
			viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.z, position.y, position.x));
			break;
		case CameraComponent::ProjectionType::PERSPECTIVE:
			projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, clippingPlanes.x, clippingPlanes.y);
			viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), { 0.0f, 0.0f, 1.0f });
			viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), { 0.0f, 1.0f, 0.0f });
			viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), { 1.0f, 0.0f, 0.0f });
			viewMatrix = glm::translate(viewMatrix, glm::vec3{ -position.x, -position.y, -position.z });
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
