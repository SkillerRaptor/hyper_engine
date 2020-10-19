#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <unordered_map>
#include <queue>

#include "Rendering/ShaderManager.hpp"

namespace Hyperion
{
	struct OpenGLShaderData : public ShaderData
	{
		uint32_t ShaderId;
		std::unordered_map<std::string, uint32_t> UniformCache = {};
	};

	class OpenGLShaderManager : public ShaderManager
	{
	private:
		std::unordered_map<uint32_t, OpenGLShaderData> m_Shaders;
		std::queue<uint32_t> m_ShaderIds;

	public:
		OpenGLShaderManager();
		~OpenGLShaderManager();

		virtual uint32_t CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "") override;
		virtual bool UseShader(uint32_t handle) override;
		virtual bool DeleteShader(uint32_t handle) override;

		virtual void SetInteger(uint32_t handle, const std::string& name, int value) override;
		virtual void SetUnsignedInteger(uint32_t handle, const std::string& name, unsigned int value) override;
		virtual void SetIntegerArray(uint32_t handle, const std::string& name, size_t count, int* values) override;

		virtual void SetFloat(uint32_t handle, const std::string& name, float value) override;
		virtual void SetFloatArray(uint32_t handle, const std::string& name, size_t count, float* values) override;

		virtual void SetVector2(uint32_t handle, const std::string& name, float x, float y) override;
		virtual void SetVector2(uint32_t handle, const std::string& name, const Vec2& vector) override;

		virtual void SetVector3(uint32_t handle, const std::string& name, float x, float y, float z) override;
		virtual void SetVector3(uint32_t handle, const std::string& name, const Vec3& vector) override;

		virtual void SetVector4(uint32_t handle, const std::string& name, float x, float y, float z, float w) override;
		virtual void SetVector4(uint32_t handle, const std::string& name, const Vec4& vector) override;

		virtual void SetMatrix2(uint32_t handle, const std::string& name, const Mat2& matrix) override;
		virtual void SetMatrix3(uint32_t handle, const std::string& name, const Mat3& matrix) override;
		virtual void SetMatrix4(uint32_t handle, const std::string& name, const Mat4& matrix) override;

		virtual std::string GetVertexShaderPath(uint32_t handle) override;
		virtual std::string GetFragmentShaderPath(uint32_t handle) override;
		virtual std::string GetGeometryShaderPath(uint32_t handle) override;

	private:
		bool GenerateShader(OpenGLShaderData& shaderData, const char* vertexCode, const char* fragmentCode, const char* geometryCode);
		bool CheckShaderErrors(uint32_t id, uint32_t shader);
		bool CheckProgramErrors(OpenGLShaderData& shaderData);
		uint32_t GetUniformLocation(OpenGLShaderData& shaderData, std::string name);
	};
}
