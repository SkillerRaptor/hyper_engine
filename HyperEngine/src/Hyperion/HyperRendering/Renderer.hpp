#pragma once

#include "ShaderManager.hpp"
#include "TextureManager.hpp"

namespace Hyperion
{
	class Renderer
	{
	protected:
		ShaderManager* m_ShaderManager;
		TextureManager* m_TextureManager;
	};
}
