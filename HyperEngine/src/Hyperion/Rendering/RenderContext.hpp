#pragma once

#include "ShaderManager.hpp"
#include "TextureManager.hpp"

namespace Hyperion
{
	class RenderContext
	{
	protected:
	public:
		RenderContext() = default;
		~RenderContext() = default;

		virtual ShaderManager* GetShaderManager() = 0;
		virtual TextureManager* GetTextureManager() = 0;
	};
}