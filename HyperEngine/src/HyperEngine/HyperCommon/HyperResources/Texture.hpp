#pragma once

#include <cstdint>

namespace HyperEngine
{
	struct Texture
	{
		static constexpr const uint32_t INDEX_MASK = ((1u << 16) - 1);
		static constexpr const uint32_t VERSION_MASK = ~((1u << 16) - 1);
		
		uint32_t handle{ 0 };
		
		inline bool IsHandleValid() const { return handle != 0; }
		
		inline uint16_t GetIndex() const { return (handle & INDEX_MASK) >> 0; }
		inline uint16_t GetVersion() const { return (handle & VERSION_MASK) >> 16; }
		
		inline bool operator==(const Texture& texture) const { return handle == texture.handle; }
		inline bool operator!=(const Texture& texture) const { return handle != texture.handle; }
	};
	
	struct CubemapTexture
	{
		static constexpr const uint32_t INDEX_MASK = ((1u << 16) - 1);
		static constexpr const uint32_t VERSION_MASK = ~((1u << 16) - 1);
		
		uint32_t handle{ 0 };
		
		inline bool IsHandleValid() const { return handle != 0; }
		
		inline uint16_t GetIndex() const { return (handle & INDEX_MASK) >> 0; }
		inline uint16_t GetVersion() const { return (handle & VERSION_MASK) >> 16; }
		
		inline bool operator==(const CubemapTexture& cubemapTexture) const { return handle == cubemapTexture.handle; }
		inline bool operator!=(const CubemapTexture& cubemapTexture) const { return handle != cubemapTexture.handle; }
	};
}