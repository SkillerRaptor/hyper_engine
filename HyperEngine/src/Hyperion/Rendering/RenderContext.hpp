#pragma once

#include "Renderer2D.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"

namespace Hyperion
{
	struct FrameSize
	{
		uint32_t XPos;
		uint32_t YPos;

		uint32_t Width;
		uint32_t Height;
	};

	class RenderContext
	{
	protected:
		FrameSize m_FrameSize;

	public:
		RenderContext()
			: m_FrameSize({ 0, 0, 0, 0 }) {}
		virtual ~RenderContext() = default;

		virtual FrameSize& GetFrameSize() { return m_FrameSize; }

		virtual Renderer2D* GetRenderer2D() = 0;
		virtual ShaderManager* GetShaderManager() = 0;
		virtual TextureManager* GetTextureManager() = 0;
	};
}
