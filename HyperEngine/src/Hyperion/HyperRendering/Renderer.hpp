#pragma once

#include "SceneRecorder.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"
#include "HyperCore/Core.hpp"

namespace Hyperion
{
	class Renderer
	{
	protected:
		Ref<ShaderManager> m_ShaderManager;
		Ref<TextureManager> m_TextureManager;
		Ref<SceneRecorder> m_SceneRecorder;
	};
}
