#pragma once

#include <string>
#include <unordered_map>
#include <queue>

#include "Shader.hpp"
#include "Texture.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NonMoveable.hpp"

namespace Hyperion
{
	class ResourceManager : public NonCopyable, NonMoveable 
	{
	private:
		static std::unordered_map<int, Shader> m_ShaderCache;
		static std::unordered_map<int, Texture> m_TextureCache;
		static std::queue<int> m_ShaderIds;
		static std::queue<int> m_TextureIds;

	public:
		ResourceManager() = delete;
		~ResourceManager() = delete;

		static int LoadTexture(TextureType textureType, const char* filePath, bool alpha = false);
		static int LoadShader(const char* vertexShader, const char* fragmentShader, const char* geometryShader = nullptr);

		static void SetTexture(int textureId, Texture texture);
		static void SetShader(int shaderId, Shader shader);

		static void DeleteTexture(int textureId);
		static void DeleteShader(int shaderId);

		static Texture& GetTexture(int textureId);
		static Shader& GetShader(int shaderId);
	};
}