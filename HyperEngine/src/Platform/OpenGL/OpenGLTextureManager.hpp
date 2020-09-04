#pragma once

#include "OpenGLTexture.hpp"
#include "Rendering/TextureManager.hpp"

namespace Hyperion
{
	class OpenGLTextureManager : public TextureManager
	{
	private:
		std::unordered_map<uint32_t, OpenGLTexture> m_Textures;

	public:
		OpenGLTextureManager();
		~OpenGLTextureManager();

		virtual uint32_t CreateTexture(const std::string& path, TextureType textureType = TextureType::DEFAULT) override;
		virtual uint32_t CreateTexture(uint32_t width, uint32_t height, TextureType textureType = TextureType::DEFAULT) override;
		virtual bool BindTexture(uint32_t textureIndex, uint32_t textureSlot) override;
		virtual bool DeleteTexture(uint32_t textureIndex) override;
		virtual Texture* GetTexture(uint32_t textureIndex) override;
	};
}