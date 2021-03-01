#include "HyperRendering/HyperOpenGL46/OpenGL46Renderer2D.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace HyperRendering
{
	OpenGL46Renderer2D::OpenGL46Renderer2D()
	{
		m_QuadVertexArray = HyperCore::CreateRef<OpenGL46VertexArray>(VertexLayout::Vertex2D);
		m_QuadVertexArray->Bind();

		m_QuadVertexBuffer = HyperCore::CreateRef<OpenGL46VertexBuffer>(VertexLayout::Vertex2D, MaxVertices);
		m_QuadVertexBuffer->Bind(m_QuadVertexArray->m_RendererID);

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

		m_QuadIndexBuffer = HyperCore::CreateRef<OpenGL46IndexBuffer>(quadIndices, MaxIndices);
		m_QuadIndexBuffer->Bind(m_QuadVertexArray->m_RendererID);
		delete[] quadIndices;

		m_QuadVertexArray->Init();
	}

	OpenGL46Renderer2D::~OpenGL46Renderer2D()
	{
		delete[] m_QuadVertexBufferBase;
	}


	void OpenGL46Renderer2D::Setup()
	{
		int32_t samplers[32];
		for (uint32_t i = 0; i < MaxTextureSlots; i++)
			samplers[i] = i;

		m_ShaderManager->Use({ 65536 });
		m_ShaderManager->SetIntegerArray({ 65536 }, "u_Textures", MaxTextureSlots, samplers);

		uint32_t whiteTextureData = 0xFFFFFFFF;

		TextureHandle whiteTexture = m_TextureManager->Create(1, 1, TextureType::DEFAULT);
		m_TextureManager->Bind(whiteTexture, 0);
		m_TextureManager->SetData(whiteTexture, &whiteTextureData, sizeof(uint32_t));

		m_TextureSlots.resize(MaxTextureSlots);
		for (size_t i = 0; i < MaxTextureSlots; i++)
			m_TextureSlots[i] = TextureHandle{ 0 };
		m_TextureSlots[0] = whiteTexture;
	}

	void OpenGL46Renderer2D::BeginScene()
	{
		StartBatch();
	}

	void OpenGL46Renderer2D::EndScene()
	{
		Flush();
	}

	void OpenGL46Renderer2D::Flush()
	{
		if (m_QuadIndexCount == 0)
			return;

		uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase);
		m_QuadVertexBuffer->Bind({ 1 });
		m_QuadVertexBuffer->SetData(m_QuadVertexBufferBase, dataSize / sizeof(Vertex2D));

		m_ShaderManager->Use({ 65536 });

		for (uint32_t i = 0; i < m_TextureSlotIndex; i++)
			m_TextureManager->Bind(m_TextureSlots[i], i);

		m_QuadVertexArray->Bind();
		glDrawElements(GL_TRIANGLES, (unsigned int)m_QuadIndexCount, GL_UNSIGNED_INT, nullptr);

		m_DrawCalls++;
	}

	void OpenGL46Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color)
	{
		DrawQuad(position, rotation, scale, color, TextureHandle{ 0 });
	}

	void OpenGL46Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, TextureHandle texture)
	{
		DrawQuad(position, rotation, scale, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, texture);
	}

	void OpenGL46Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color, TextureHandle texture)
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

		glm::mat4 transform = glm::translate(glm::mat4{ 1.0f }, position) * glm::toMat4(glm::quat{ glm::radians(rotation) }) * glm::scale(glm::mat4{ 1.0f }, scale);

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

	void OpenGL46Renderer2D::StartBatch()
	{
		m_QuadCount = 0;
		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase;
	}

	void OpenGL46Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void OpenGL46Renderer2D::SetCamera(const glm::vec3& position, const glm::vec3& rotation, float fov, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, HyperECS::CameraComponent::ProjectionType projectionType)
	{
		glm::mat4 projectionMatrix{ 1.0f };
		glm::mat4 viewMatrix{ 1.0f };

		float aspectRatio{ 0.0f };
		TextureHandle gameColorTexture = m_SceneRecorder->GetGameColorAttachment();

		if (gameColorTexture.IsHandleValid())
		{
			uint32_t width = m_TextureManager->GetWidth(gameColorTexture).value_or(0);
			uint32_t height = m_TextureManager->GetHeight(gameColorTexture).value_or(0);
			aspectRatio = (static_cast<float>(width) * viewportRect.x) / (static_cast<float>(height) * viewportRect.y);
		}

		switch (projectionType)
		{
		case HyperECS::CameraComponent::ProjectionType::ORTHOGRAPHIC:
			projectionMatrix = glm::ortho(-aspectRatio * fov, aspectRatio * fov, -fov, fov, clippingPlanes.x, clippingPlanes.y);
			viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.z, position.y, position.x));
			break;
		case HyperECS::CameraComponent::ProjectionType::PERSPECTIVE:
			projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, clippingPlanes.x, clippingPlanes.y);
			viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.z), { 0.0f, 0.0f, 1.0f });
			viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.y), { 0.0f, 1.0f, 0.0f });
			viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation.x), { 1.0f, 0.0f, 0.0f });
			viewMatrix = glm::translate(viewMatrix, glm::vec3{ -position.x, -position.y, -position.z });
			break;
		default:
			break;
		}

		m_ShaderManager->Use({ 65536 });
		m_ShaderManager->SetMatrix4({ 65536 }, "u_ProjectionMatrix", projectionMatrix);
		m_ShaderManager->SetMatrix4({ 65536 }, "u_ViewMatrix", viewMatrix);
	}

	void OpenGL46Renderer2D::SetCamera(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
	{
		m_ShaderManager->Use({ 65536 });
		m_ShaderManager->SetMatrix4({ 65536 }, "u_ProjectionMatrix", projectionMatrix);
		m_ShaderManager->SetMatrix4({ 65536 }, "u_ViewMatrix", viewMatrix);
	}
}
