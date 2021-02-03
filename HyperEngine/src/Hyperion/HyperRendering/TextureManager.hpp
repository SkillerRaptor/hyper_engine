#pragma once

#include <optional>
#include <queue>
#include <string>

#include "HyperUtilities/NonCopyable.hpp"
#include "HyperUtilities/NonMoveable.hpp"

namespace Hyperion
{
	enum class TextureType
	{
		COMPUTE,
		DEFAULT,
		DIFFUSE,
		COLOR,
		DEPTH,
		STENCIL,
		DEPTH_STENCIL,
		HEIGHT,
		NORMAL,
		SPECULAR,
		PREBUILT
	};

	struct TextureData
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint8_t Channels = 0;

		std::string Path = "";
		TextureType Type = TextureType::DEFAULT;
	};

	struct TextureHandle
	{
		uint32_t Handle;

		bool IsHandleValid() const { return Handle != 0; }
		bool operator==(const TextureHandle& textureHandle) const { return Handle == textureHandle.Handle; }
	};

	struct TextureHandleHasher
	{
		size_t operator()(const TextureHandle& textureHandle) const { return (std::hash<uint32_t>()(textureHandle.Handle)); }
	};

	class TextureManager : public NonCopyable, NonMoveable
	{
	protected:
		std::queue<TextureHandle> m_TextureIds;

	public:
		TextureManager() = default;
		virtual ~TextureManager() = default;

		unsigned char* LoadImage(const std::string& path, int32_t& width, int32_t& height, int32_t& channels);
		void FreeImage(unsigned char* pixels);

		virtual TextureHandle CreateTexture(const std::string& path, TextureType textureType = TextureType::DEFAULT) = 0;
		virtual TextureHandle CreateTexture(uint32_t width, uint32_t height, TextureType textureType = TextureType::DEFAULT) = 0;
		virtual void BindTexture(TextureHandle handle, uint32_t textureSlot) = 0;
		virtual void DeleteTexture(TextureHandle handle) = 0;

		virtual void SetWidth(TextureHandle handle, uint32_t width) = 0;
		virtual std::optional<uint32_t> GetWidth(TextureHandle handle) const = 0;

		virtual void SetHeight(TextureHandle handle, uint32_t height) = 0;
		virtual std::optional<uint32_t> GetHeight(TextureHandle handle) const = 0;

		virtual void SetTextureType(TextureHandle handle, TextureType textureType) = 0;
		virtual std::optional<TextureType> GetTextureType(TextureHandle handle) const = 0;

		virtual void SetTexturePixels(TextureHandle handle, void* pixels, uint32_t size) = 0;

		virtual std::optional<uint8_t> GetColorChannels(TextureHandle handle) const = 0;
		virtual std::optional<std::string> GetFilePath(TextureHandle handle) const = 0;
		
		virtual void* GetImageTextureId(TextureHandle handle) const = 0;

	protected:
		virtual void GenerateTexture(TextureData* textureData, unsigned char* pixels = nullptr) = 0;
	};
}
