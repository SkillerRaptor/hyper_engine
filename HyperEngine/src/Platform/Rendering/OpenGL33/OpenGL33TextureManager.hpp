#pragma once

#include <unordered_map>
#include <queue>

#include "HyperRendering/TextureManager.hpp"

namespace Hyperion
{
	struct OpenGL33TextureData : public TextureData
	{
		uint32_t TextureId = -1;
	};

	class OpenGL33TextureManager : public TextureManager
	{
	private:
		std::unordered_map<TextureHandle, OpenGL33TextureData, TextureHandleHasher> m_Textures;

	public:
		OpenGL33TextureManager();
		~OpenGL33TextureManager();

		virtual TextureHandle CreateTexture(const std::string& path, TextureType textureType = TextureType::DEFAULT) override;
		virtual TextureHandle CreateTexture(uint32_t width, uint32_t height, TextureType textureType = TextureType::DEFAULT) override;
		virtual void BindTexture(TextureHandle handle, uint32_t textureSlot) override;
		virtual void DeleteTexture(TextureHandle handle) override;

		virtual void SetWidth(TextureHandle handle, uint32_t width) override;
		virtual std::optional<uint32_t> GetWidth(TextureHandle handle) const override;

		virtual void SetHeight(TextureHandle handle, uint32_t height) override;
		virtual std::optional<uint32_t> GetHeight(TextureHandle handle) const override;

		virtual void SetTextureType(TextureHandle handle, TextureType textureType) override;
		virtual std::optional<TextureType> GetTextureType(TextureHandle handle) const override;

		virtual void SetTexturePixels(TextureHandle handle, void* pixels, uint32_t size) override;

		virtual std::optional<uint8_t> GetColorChannels(TextureHandle handle) const override;
		virtual std::optional<std::string> GetFilePath(TextureHandle handle) const override;

		virtual void* GetImageTextureId(TextureHandle handle) const override;

	protected:
		virtual void GenerateTexture(TextureData* textureData, unsigned char* pixels = nullptr) override;
	};
}
