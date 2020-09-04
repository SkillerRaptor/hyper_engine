#pragma once

#include "OpenGLShaderManager.hpp"
#include "OpenGLTextureManager.hpp"
#include "Rendering/RenderContext.hpp"

namespace Hyperion
{
	class OpenGLContext : public RenderContext
	{
	private:
		OpenGLShaderManager m_ShaderManager;
		OpenGLTextureManager m_TextureManager;

	public:
		OpenGLContext();
		~OpenGLContext();
		
		virtual void Init() override;
		virtual ShaderManager* GetShaderManager() override;
		virtual TextureManager* GetTextureManager() override;
	};
}