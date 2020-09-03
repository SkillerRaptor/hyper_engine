#include "ResourceManager.hpp"

#include "Utilities/Log.hpp"

namespace Hyperion
{
	std::unordered_map<int, Texture> ResourceManager::m_TextureCache = {};
	std::unordered_map<int, Shader> ResourceManager::m_ShaderCache = {};
	std::queue<int> ResourceManager::m_ShaderIds = {};
	std::queue<int> ResourceManager::m_TextureIds = {};

	int ResourceManager::LoadTexture(TextureType textureType, const char* filePath, bool alpha)
	{
		Texture texture;
		if (texture.LoadTexture(textureType, filePath, alpha))
		{
			//ResourceManager::m_TextureCache[name] = texture;
			//HP_CORE_DEBUG("Texture % loaded...", name);
		}
		else
		{
			HP_CORE_DEBUG("Texture not loaded...");
			return -1;
		}
	}

	int ResourceManager::LoadShader(const char* vertexShader, const char* fragmentShader, const char* geometryShader)
	{
		Shader shader;
		if (shader.LoadShader(vertexShader, fragmentShader, geometryShader))
		{
			//ResourceManager::m_ShaderCache[name] = shader;
			//HP_CORE_DEBUG("Shader % loaded...", name);
		}
		else
		{
			HP_CORE_DEBUG("Shader not loaded...");
			return -1;
		}
	}

	void ResourceManager::SetTexture(int textureId, Texture texture)
	{
		ResourceManager::m_TextureCache[textureId] = texture;
	}

	void ResourceManager::SetShader(int shaderId, Shader shader)
	{
		ResourceManager::m_ShaderCache[shaderId] = shader;
	}

	void ResourceManager::DeleteTexture(int textureId)
	{
		ResourceManager::m_TextureCache.erase(textureId);
	}

	void ResourceManager::DeleteShader(int shaderId)
	{
		ResourceManager::m_ShaderCache.erase(shaderId);
	}

	Texture& ResourceManager::GetTexture(int textureId)
	{
		return ResourceManager::m_TextureCache[textureId];
	}

	Shader& ResourceManager::GetShader(int shaderId)
	{
		return ResourceManager::m_ShaderCache[shaderId];
	}
}