#pragma once

#include <optional>
#include <queue>
#include <string>

#include "HyperUtilities/NonCopyable.hpp"
#include "HyperUtilities/NonMoveable.hpp"

namespace HyperRendering
{
	enum class TextureType : uint16_t
	{
		COLOR,
		COMPUTE,
		DEFAULT,
		DEPTH,
		DEPTH_STENCIL,
		DIFFUSE,
		HEIGHT,
		NORMAL,
		PREBUILT,
		SPECULAR
	};

	struct TextureHandle
	{
		uint32_t Handle{ 0 };

		inline bool IsHandleValid() const
		{
			return Handle != 0;
		}
		
		inline uint16_t GetIndex() const
		{
			static constexpr const uint32_t INDEX_MASK = ((1u << 16) - 1);
			return (Handle & INDEX_MASK) >> 0;
		}
		
		inline uint16_t GetVersion() const
		{
			static constexpr const uint32_t VERSION_MASK = ~((1u << 16) - 1);
			return (Handle & VERSION_MASK) >> 16;
		}

		inline bool operator==(const TextureHandle& textureHandle) const
		{
			return Handle == textureHandle.Handle;
		}
	};

	class TextureManager : public HyperUtilities::NonCopyable, HyperUtilities::NonMoveable
	{
	public:
		TextureManager() = default;
		virtual ~TextureManager() = default;

		/* Todo: Assets Manager */
		unsigned char* LoadImage(const std::string& path, int32_t& width, int32_t& height, int32_t& channels);
		void FreeImage(unsigned char* pixels);

		virtual TextureHandle Create(const std::string& path, TextureType type = TextureType::DEFAULT) = 0;
		virtual TextureHandle Create(uint32_t width, uint32_t height, TextureType textureType = TextureType::DEFAULT) = 0;
		virtual void Delete(TextureHandle handle) = 0;
		
		virtual void Bind(TextureHandle handle, uint32_t textureSlot) = 0;

		virtual void SetWidth(TextureHandle handle, uint32_t width) = 0;
		virtual std::optional<uint32_t> GetWidth(TextureHandle handle) const = 0;

		virtual void SetHeight(TextureHandle handle, uint32_t height) = 0;
		virtual std::optional<uint32_t> GetHeight(TextureHandle handle) const = 0;

		virtual void SetType(TextureHandle handle, TextureType type) = 0;
		virtual std::optional<TextureType> GetType(TextureHandle handle) const = 0;

		virtual void SetData(TextureHandle handle, void* pixels, uint32_t size) = 0;

		virtual std::optional<uint8_t> GetChannels(TextureHandle handle) const = 0;

		virtual void* GetImageId(TextureHandle handle) const = 0;
	};
}
