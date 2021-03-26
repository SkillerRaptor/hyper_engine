#pragma once

#include "Shader.hpp"
#include "Texture.hpp"

namespace HyperEngine
{
	namespace AssetManager
	{
		Shader LoadShader(const char* szVertexShaderPath, const char* szFragmentShaderPath);
		Shader LoadShader(const char* szVertexShaderPath, const char* szFragmentShaderPath, const char* szGeometryPath);
		
		ComputeShader LoadComputeShader(const char* szComputeShaderPath);
		
		Texture LoadTexture(const char* szTexturePath);
	};
}