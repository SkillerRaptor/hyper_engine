#pragma once

#include <unordered_map>
#include <queue>

#include "HyperRendering/ShaderManager.hpp"

namespace Hyperion
{
	struct OpenGLShaderData : public ShaderData
	{
		uint32_t ShaderId;
		std::unordered_map<std::string, uint32_t> UniformCache = {};
	};

	class OpenGL33ShaderManager : public ShaderManager
	{
	private:
		std::unordered_map<ShaderHandle, OpenGLShaderData, ShaderHandleHasher> m_Shaders;

	public:
		OpenGL33ShaderManager();
		~OpenGL33ShaderManager();

		virtual ShaderHandle CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "") override;
		virtual bool UseShader(ShaderHandle handle) override;
		virtual bool DeleteShader(ShaderHandle handle) override;

		virtual void SetInteger(ShaderHandle handle, const std::string& name, int value) override;
		virtual void SetUnsignedInteger(ShaderHandle handle, const std::string& name, unsigned int value) override;
		virtual void SetIntegerArray(ShaderHandle handle, const std::string& name, size_t count, int* values) override;

		virtual void SetFloat(ShaderHandle handle, const std::string& name, float value) override;
		virtual void SetFloatArray(ShaderHandle handle, const std::string& name, size_t count, float* values) override;

		virtual void SetVector2(ShaderHandle handle, const std::string& name, float x, float y) override;
		virtual void SetVector2(ShaderHandle handle, const std::string& name, const Vec2& vector) override;

		virtual void SetVector3(ShaderHandle handle, const std::string& name, float x, float y, float z) override;
		virtual void SetVector3(ShaderHandle handle, const std::string& name, const Vec3& vector) override;

		virtual void SetVector4(ShaderHandle handle, const std::string& name, float x, float y, float z, float w) override;
		virtual void SetVector4(ShaderHandle handle, const std::string& name, const Vec4& vector) override;

		virtual void SetMatrix2(ShaderHandle handle, const std::string& name, const Mat2& matrix) override;
		virtual void SetMatrix3(ShaderHandle handle, const std::string& name, const Mat3& matrix) override;
		virtual void SetMatrix4(ShaderHandle handle, const std::string& name, const Mat4& matrix) override;

		virtual const std::string GetVertexShaderPath(ShaderHandle handle) override;
		virtual const std::string GetFragmentShaderPath(ShaderHandle handle) override;
		virtual const std::string GetGeometryShaderPath(ShaderHandle handle) override;

	private:
		bool GenerateShader(OpenGLShaderData& shaderData, const char* vertexCode, const char* fragmentCode, const char* geometryCode);
		bool CheckShaderErrors(uint32_t id, uint32_t shader);
		bool CheckProgramErrors(OpenGLShaderData& shaderData);
		uint32_t GetUniformLocation(OpenGLShaderData& shaderData, const std::string& name);
	};
}
