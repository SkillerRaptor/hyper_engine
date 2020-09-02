#pragma once

#include <string>
#include <unordered_map>

#include "Shader.h"
#include "Texture.h"
#include "Utilities/NonCopyable.h"
#include "Utilities/NonMoveable.h"

namespace Hyperion
{
	class ResourceManager : public NonCopyable, NonMoveable 
	{
	private:
		static std::unordered_map<std::string, Shader> m_ShadersCache;
		static std::unordered_map<std::string, Texture> m_TexturesCache;

	public:
		ResourceManager() = delete;
		~ResourceManager() = delete;

		static Texture LoadTexture(std::string name, TextureType textureType, const char* filePath, bool alpha = false);
		static Shader LoadShader(std::string name, const char* vertexShader, const char* fragmentShader, const char* geometryShader = nullptr);

		static void SetTexture(std::string name, Texture texture);
		static void SetShader(std::string name, Shader shader);

		static Texture* GetTexture(std::string name);
		static Shader* GetShader(std::string name);
	};
}