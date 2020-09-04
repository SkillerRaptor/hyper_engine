#pragma once

#include <unordered_map>
#include <queue>

#include "Shader.hpp"

namespace Hyperion
{
	class ShaderManager
	{
	protected:
		std::queue<uint32_t> m_ShaderIds;

	public:
		ShaderManager() = default;
		virtual ~ShaderManager() = default;

		virtual uint32_t CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "") = 0;
		virtual bool UseShader(uint32_t shaderIndex) = 0;
		virtual bool DeleteShader(uint32_t shaderIndex) = 0;
		virtual Shader* GetShader(uint32_t shaderIndex) = 0;
	};
}