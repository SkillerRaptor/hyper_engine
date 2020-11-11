#pragma once

#include <unordered_map>
#include <queue>

#include "HyperRendering/TextureManager.hpp"

namespace Hyperion
{
	struct OpenGLTextureData : public TextureData
	{
		uint32_t TextureId = -1;
	};

	class OpenGL46TextureManager : public TextureManager
	{
	private:
		std::unordered_map<uint32_t, OpenGLTextureData> m_Textures;
		std::queue<uint32_t> m_TextureIds;

	public:
		OpenGL46TextureManager();
		~OpenGL46TextureManager();

		virtual uint32_t CreateTexture(const std::string& path, TextureType textureType = TextureType::DEFAULT) override;
		virtual uint32_t CreateTexture(uint32_t width, uint32_t height, TextureType textureType = TextureType::DEFAULT) override;
		virtual void GenerateTexture(TextureData* textureData, bool alpha) override;
		virtual bool BindTexture(uint32_t handle, uint32_t textureSlot) override;
		virtual bool DeleteTexture(uint32_t handle) override;
		virtual bool DeleteTextureData(uint32_t handle) override;

		virtual void SetWidth(uint32_t handle, uint32_t width) override;
		virtual uint32_t GetWidth(uint32_t handle) override;
		virtual void SetHeight(uint32_t handle, uint32_t height) override;
		virtual uint32_t GetHeight(uint32_t handle) override;
		virtual uint8_t GetChannels(uint32_t handle) override;
		virtual const unsigned char* GetData(uint32_t handle) override;
		virtual const std::string GetPath(uint32_t handle) override;
		virtual void SetTextureType(uint32_t handle, TextureType textureType) override;
		virtual TextureType GetTextureType(uint32_t handle) override;
		virtual TextureData* GetTextureData(uint32_t handle) override;
	};
}
