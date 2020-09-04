#pragma once

#include "Rendering/Texture.hpp"

namespace Hyperion
{
	class OpenGLTexture : public Texture
	{
	private:
		uint32_t m_TextureId = -1;

	public:
		OpenGLTexture() = default;
		OpenGLTexture(const std::string& path, TextureType textureType);
		OpenGLTexture(uint32_t width, uint32_t height, TextureType textureType);
		~OpenGLTexture() = default;

		virtual bool LoadTexture() override;
		virtual void GenerateTexture(bool alpha = true) override;
		virtual void DeleteTexture() override;
		virtual void BindTexture(uint32_t textureSlot) const override;
	};
}