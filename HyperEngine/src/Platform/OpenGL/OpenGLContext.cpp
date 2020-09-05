#include "OpenGLContext.hpp"

namespace Hyperion
{
	OpenGLContext::OpenGLContext()
		: m_ShaderManager(), m_TextureManager()
	{
	}

	OpenGLContext::~OpenGLContext()
	{
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