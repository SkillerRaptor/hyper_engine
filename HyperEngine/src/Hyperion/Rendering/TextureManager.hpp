#pragma once

#include "Utilities/NonCopyable.hpp"
#include "Utilities/NonMoveable.hpp"

namespace Hyperion
{
	enum class TextureType
	{
		DEFAULT,
		DIFFUSE,
		FRAMEBUFFER,
		HEIGHT,
		NORMAL,
		SPECULAR
	};

	struct TextureData
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint8_t Channels = 0;
		unsigned char* Data = nullptr;

		std::string Path = "";
		TextureType TextureType = TextureType::DEFAULT;
	};

	class TextureManager : public NonCopyable, NonMoveable
	{
	public:
		TextureManager() = default;
		virtual ~TextureManager() = default;

		virtual uint32_t CreateTexture(const std::string& path, TextureType textureType = TextureType::DEFAULT) = 0;
		virtual uint32_t CreateTexture(uint32_t width, uint32_t height, TextureType textureType = TextureType::DEFAULT) = 0;
		virtual void GenerateTexture(TextureData* textureData, bool alpha) = 0;
		virtual bool BindTexture(uint32_t handle, uint32_t textureSlot) = 0;
		virtual bool DeleteTexture(uint32_t handle) = 0;

		virtual void SetWidth(uint32_t handle, uint32_t width) = 0;
		virtual uint32_t GetWidth(uint32_t handle) = 0;
		virtual void SetHeight(uint32_t handle, uint32_t height) = 0;
		virtual uint32_t GetHeight(uint32_t handle) = 0;
		virtual uint8_t GetChannels(uint32_t handle) = 0;
		virtual const unsigned char* GetData(uint32_t handle) = 0;
		virtual const std::string GetPath(uint32_t handle) = 0;
		virtual void SetTextureType(uint32_t handle, TextureType textureType) = 0;
		virtual TextureType GetTextureType(uint32_t handle) = 0;
		virtual TextureData* GetTextureData(uint32_t handle) = 0;
	};
}