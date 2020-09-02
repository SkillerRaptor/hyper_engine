#include "ResourceManager.hpp"

#include "Utilities/Log.hpp"

namespace Hyperion
{
	std::unordered_map<std::string, Texture> ResourceManager::m_TexturesCache = {};
	std::unordered_map<std::string, Shader> ResourceManager::m_ShadersCache = {};

	Texture ResourceManager::LoadTexture(std::string name, TextureType textureType, const char* filePath, bool alpha)
	{
		Texture texture;
		if (texture.LoadTexture(textureType, filePath, alpha))
		{
			ResourceManager::m_TexturesCache[name] = texture;
			HP_CORE_DEBUG("Texture % loaded...", name);
		}
		else
			HP_CORE_DEBUG("Texture % not loaded...", name);
		return texture;
	}

	Shader ResourceManager::LoadShader(std::string name, const char* vertexShader, const char* fragmentShader, const char* geometryShader)
	{
		Shader shader;
		if (shader.LoadShader(vertexShader, fragmentShader, geometryShader))
		{
			ResourceManager::m_ShadersCache[name] = shader;
			HP_CORE_DEBUG("Shader % loaded...", name);
		}
		else
			HP_CORE_DEBUG("Shader % not loaded...", name);
		return shader;
	}

	void ResourceManager::SetTexture(std::string name, Texture texture)
	{
		ResourceManager::m_TexturesCache[name] = texture;
	}

	void ResourceManager::SetShader(std::string name, Shader shader)
	{
		ResourceManager::m_ShadersCache[name] = shader;
	}

	Texture* ResourceManager::GetTexture(std::string name)
	{
		return &ResourceManager::m_TexturesCache[name];
	}

	Shader* ResourceManager::GetShader(std::string name)
	{
		return &ResourceManager::m_ShadersCache[name];
	}
}