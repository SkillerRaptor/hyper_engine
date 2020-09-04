#pragma once

#include "OpenGLShader.hpp"
#include "Rendering/ShaderManager.hpp"

namespace Hyperion
{
	class OpenGLShaderManager : public ShaderManager
	{
	private:
		std::unordered_map<uint32_t, OpenGLShader> m_Shaders;

	public:
		OpenGLShaderManager();
		~OpenGLShaderManager();

		virtual uint32_t CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "") override;
		virtual bool UseShader(uint32_t shaderIndex) override;
		virtual bool DeleteShader(uint32_t shaderIndex) override;
		virtual Shader* GetShader(uint32_t shaderIndex) override;
	};
}