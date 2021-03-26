#pragma once

#include <cstdint>

namespace HyperEngine
{
	struct Shader
	{
		static constexpr const uint32_t INDEX_MASK = ((1u << 16) - 1);
		static constexpr const uint32_t VERSION_MASK = ~((1u << 16) - 1);
		
		uint32_t handle{ 0 };
		
		inline bool IsHandleValid() const { return handle != 0; }
		
		inline uint16_t GetIndex() const { return (handle & INDEX_MASK) >> 0; }
		inline uint16_t GetVersion() const { return (handle & VERSION_MASK) >> 16; }
		
		inline bool operator==(const Shader& shader) const { return handle == shader.handle; }
		inline bool operator!=(const Shader& shader) const { return handle != shader.handle; }
	};
	
	struct ComputeShader
	{
		static constexpr const uint32_t INDEX_MASK = ((1u << 16) - 1);
		static constexpr const uint32_t VERSION_MASK = ~((1u << 16) - 1);
		
		uint32_t handle{ 0 };
		
		inline bool IsHandleValid() const { return handle != 0; }
		
		inline uint16_t GetIndex() const { return (handle & INDEX_MASK) >> 0; }
		inline uint16_t GetVersion() const { return (handle & VERSION_MASK) >> 16; }
		
		inline bool operator==(const ComputeShader& computeShader) const { return handle == computeShader.handle; }
		inline bool operator!=(const ComputeShader& computeShader) const { return handle != computeShader.handle; }
	};
}