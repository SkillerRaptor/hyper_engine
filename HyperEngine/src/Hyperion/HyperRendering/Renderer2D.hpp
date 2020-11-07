#pragma once

#include "Renderer.hpp"

namespace Hyperion
{
	class Renderer2D : public Renderer
	{
	protected:
		Vec4 m_QuadVertexPositions[4];

		const uint32_t MaxQuads = 20000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		uint32_t m_QuadCount;
		uint32_t m_QuadIndexCount;
		uint32_t m_DrawCalls;

	public:
		Renderer2D() = default;
		virtual ~Renderer2D() = default;

		virtual void BeginScene(uint32_t width, uint32_t height, float zoom, float nearPlane, float farPlane, const Vec3& position) = 0;
		virtual void EndScene() = 0;

		virtual void Flush() = 0;
		virtual void FlushAndReset() = 0;

		virtual void DrawQuad(const Vec3& position, const Vec3& rotation, const Vec3& scale, const Vec4& color) = 0;

		virtual void SetShaderManager(Ref<ShaderManager> shaderManager) = 0;
		virtual void SetTextureManager(Ref<TextureManager> textureManager) = 0;
	};
}
