#include "OpenGLContext.hpp"

namespace Hyperion
{
	OpenGLContext::OpenGLContext()
	{
	}

	OpenGLContext::~OpenGLContext()
	{
	}

	void OpenGLContext::Init()
	{
		m_ShaderManager = OpenGLShaderManager();
		m_TextureManager = OpenGLTextureManager();
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