#pragma once

#include "HyperCore/Core.hpp"
#include "HyperRendering/SceneRecorder.hpp"
#include "HyperRendering/ShaderManager.hpp"
#include "HyperRendering/TextureManager.hpp"

namespace HyperRendering
{
	class Renderer
	{
	protected:
		HyperCore::Ref<ShaderManager> m_ShaderManager;
		HyperCore::Ref<TextureManager> m_TextureManager;
		HyperCore::Ref<SceneRecorder> m_SceneRecorder;
	};
}
