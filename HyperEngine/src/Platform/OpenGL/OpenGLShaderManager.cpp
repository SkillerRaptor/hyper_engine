#include "OpenGLShaderManager.hpp"

#include "Utilities/Log.hpp"

namespace Hyperion
{
	OpenGLShaderManager::OpenGLShaderManager()
	{
		HP_CORE_DEBUG("OpenGL Shader-Manager loaded...");
	}

	OpenGLShaderManager::~OpenGLShaderManager()
	{
		HP_CORE_DEBUG("OpenGL Shader-Manager unloaded...");
		//for (std::unordered_map<uint32_t, Shader*>::iterator it = m_Shaders.begin(); it != m_Shaders.end(); it++)
		//	it->second->DeleteShader();
		m_Shaders.clear();
	}

	uint32_t OpenGLShaderManager::CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
	{
		OpenGLShader shader = OpenGLShader(vertexPath, fragmentPath, geometryPath);
		if (!shader.LoadShader())
		{
			HP_CORE_DEBUG("Shader not loaded...");
			return -1;
		}

		uint32_t shaderId = 0;
		if (!m_ShaderIds.empty())
		{
			shaderId = m_ShaderIds.front();
			m_ShaderIds.pop();
		}
		else
			shaderId = static_cast<uint32_t>(m_Shaders.size());
		m_Shaders.emplace(shaderId, std::move(shader));
		HP_CORE_DEBUG("Shader % loaded...");
		return shaderId;
	}

	bool OpenGLShaderManager::UseShader(uint32_t shaderIndex)
	{
		if (m_Shaders.find(shaderIndex) == m_Shaders.end())
			return false;
		m_Shaders[shaderIndex].UseShader();
		return true;
	}

	bool OpenGLShaderManager::DeleteShader(uint32_t shaderIndex)
	{
		if (m_Shaders.find(shaderIndex) == m_Shaders.end())
			return false;
		m_Shaders[shaderIndex].DeleteShader();
		m_Shaders.erase(shaderIndex);
		m_ShaderIds.push(shaderIndex);
		return true;
	}

	Shader* OpenGLShaderManager::GetShader(uint32_t shaderIndex)
	{
		if (m_Shaders.find(shaderIndex) == m_Shaders.end())
			return nullptr;
		return static_cast<Shader*>(&m_Shaders[shaderIndex]);
	}
}