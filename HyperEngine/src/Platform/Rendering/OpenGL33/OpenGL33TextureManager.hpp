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
		virtual void GenerateTexture(TextureData* textureData, unsigned char* pixels = nullptr) override;
		virtual bool BindTexture(TextureHandle handle, uint32_t textureSlot) override;
		virtual bool DeleteTexture(TextureHandle handle) override;
		virtual bool DeleteTextureData(TextureHandle handle) override;

		virtual void SetWidth(TextureHandle handle, uint32_t width) override;
		virtual uint32_t GetWidth(TextureHandle handle) override;

		virtual void SetHeight(TextureHandle handle, uint32_t height) override;
		virtual uint32_t GetHeight(TextureHandle handle) override;

		virtual void SetTextureType(TextureHandle handle, TextureType textureType) override;
		virtual TextureType GetTextureType(TextureHandle handle) override;

		virtual uint8_t GetChannels(TextureHandle handle) override;
		virtual const std::string GetPath(TextureHandle handle) override;
		virtual TextureData* GetTextureData(TextureHandle handle) override;

		virtual void* GetImageTextureId(TextureHandle handle) override;
	};
}
