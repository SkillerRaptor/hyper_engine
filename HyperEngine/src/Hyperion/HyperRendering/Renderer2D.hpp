#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.hpp"

namespace Hyperion
{
	struct CameraInfo
	{
		glm::vec3 Position;

		size_t Width;
		size_t Height;

		float Zoom;
		float NearPlane;
		float FarPlane;
	};

	class Renderer2D : public Renderer
	{
	protected:
		glm::vec4 m_QuadVertexPositions[4];

		const uint32_t MaxQuads = 20000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		uint32_t m_QuadCount = 0;
		uint32_t m_QuadIndexCount = 0;
		uint32_t m_DrawCalls = 0;

	public:
		Renderer2D() = default;
		virtual ~Renderer2D() = default;

		virtual void BeginScene() = 0;
		virtual void EndScene() = 0;

		virtual void Flush() = 0;

		virtual void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color) = 0;

		virtual void SetShaderManager(Ref<ShaderManager> shaderManager) = 0;
		virtual void SetTextureManager(Ref<TextureManager> textureManager) = 0;

	protected:
		virtual void StartBatch() = 0;
		virtual void NextBatch() = 0;
	};
}
