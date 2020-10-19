#include "OpenGLContext.hpp"

namespace Hyperion
{
	OpenGLContext::OpenGLContext()
		: m_Renderer2D(), m_ShaderManager(), m_TextureManager()
	{
	}

	OpenGLContext::~OpenGLContext()
	{
	}

	Renderer2D* OpenGLContext::GetRenderer2D()
	{
		return static_cast<Renderer2D*>(&m_Renderer2D);
	}

	ShaderManager* OpenGLContext::GetShaderManager()
	{
		return static_cast<ShaderManager*>(&m_ShaderManager);
	}

	TextureManager* OpenGLContext::GetTextureManager()
	{
		return static_cast<TextureManager*>(&m_TextureManager);
	}
}
