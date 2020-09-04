#pragma once

#include <unordered_map>
#include <queue>

#include "Texture.hpp"

namespace Hyperion
{
	class TextureManager
	{
	protected:
		std::queue<uint32_t> m_TextureIds;

	public:
		TextureManager() = default;
		virtual ~TextureManager() = default;

		virtual uint32_t CreateTexture(const std::string& path, TextureType textureType = TextureType::DEFAULT) = 0;
		virtual uint32_t CreateTexture(uint32_t width, uint32_t height, TextureType textureType = TextureType::DEFAULT) = 0;
		virtual bool BindTexture(uint32_t textureIndex, uint32_t textureSlot) = 0;
		virtual bool DeleteTexture(uint32_t textureIndex) = 0;
		virtual Texture* GetTexture(uint32_t textureIndex) = 0;
	};
}