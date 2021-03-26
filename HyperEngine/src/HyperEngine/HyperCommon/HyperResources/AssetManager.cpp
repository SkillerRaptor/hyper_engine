#include "AssetManager.hpp"

#include <HyperResources/ShaderFactory.hpp>
#include <HyperResources/TextureFactory.hpp>

namespace HyperEngine
{
	namespace AssetManager
	{
		Shader LoadShader(const char* szVertexShaderPath, const char* szFragmentShaderPath)
		{
			(void) szVertexShaderPath;
			(void) szFragmentShaderPath;
			return {};
		}
		
		Shader LoadShader(
			const char* szVertexShaderPath,
			const char* szFragmentShaderPath,
			const char* szGeometryPath)
		{
			(void) szVertexShaderPath;
			(void) szFragmentShaderPath;
			(void) szGeometryPath;
			return {};
		}
		
		ComputeShader LoadComputeShader(const char* szComputeShaderPath)
		{
			(void) szComputeShaderPath;
			return {};
		}
		
		Texture LoadTexture(const char* szTexturePath)
		{
			(void) szTexturePath;
			return {};
		}
	}
}