#pragma once

#include "OpenGLRenderer2D.hpp"
#include "OpenGLShaderManager.hpp"
#include "OpenGLTextureManager.hpp"
#include "Rendering/RenderContext.hpp"

namespace Hyperion
{
	class OpenGLContext : public RenderContext
	{
	private:
		OpenGLRenderer2D m_Renderer2D;
		OpenGLShaderManager m_ShaderManager;
		OpenGLTextureManager m_TextureManager;

	public:
		OpenGLContext();
		~OpenGLContext();
		
		virtual Renderer2D* GetRenderer2D() override;
		virtual ShaderManager* GetShaderManager() override;
		virtual TextureManager* GetTextureManager() override;
	};
}
