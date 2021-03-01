#pragma once

#include <unordered_map>
#include <queue>

#include "HyperMemory/SparseMemoryPool.hpp"
#include "HyperRendering/TextureManager.hpp"

namespace HyperRendering
{
	class OpenGL33TextureManager : public TextureManager
	{
	private:
		struct TextureData
		{
			uint16_t MagicNumber;
			
			uint32_t Width;
			uint32_t Height;
			uint8_t Channels;
			
			TextureType Type;
			
			uint32_t Id{ UINT32_MAX };
		};
		
		HyperMemory::SparseMemoryPool<TextureData> m_TextureStorage;
		uint32_t m_TextureVersion{ 1 };

	public:
		OpenGL33TextureManager();
		~OpenGL33TextureManager();

		virtual TextureHandle Create(const std::string& path, TextureType type = TextureType::DEFAULT) override;
		virtual TextureHandle Create(uint32_t width, uint32_t height, TextureType type = TextureType::DEFAULT) override;
		virtual void Delete(TextureHandle handle) override;
		
		virtual void Bind(TextureHandle handle, uint32_t textureSlot) override;

		virtual void SetWidth(TextureHandle handle, uint32_t width) override;
		virtual std::optional<uint32_t> GetWidth(TextureHandle handle) const override;

		virtual void SetHeight(TextureHandle handle, uint32_t height) override;
		virtual std::optional<uint32_t> GetHeight(TextureHandle handle) const override;

		virtual void SetType(TextureHandle handle, TextureType type) override;
		virtual std::optional<TextureType> GetType(TextureHandle handle) const override;

		virtual void SetData(TextureHandle handle, void* pixels, uint32_t size) override;

		virtual std::optional<uint8_t> GetChannels(TextureHandle handle) const override;

		virtual void* GetImageId(TextureHandle handle) const override;
	};
}
