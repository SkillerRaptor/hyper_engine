#pragma once

#include <unordered_map>

#include "Rendering/Shader.hpp"

namespace Hyperion
{
	class OpenGLShader : public Shader
	{
	private:
		uint32_t m_ShaderId = -1;
		std::unordered_map<std::string, uint32_t> m_UniformCache = {};

	public:
		OpenGLShader() = default;
		OpenGLShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader);
		~OpenGLShader() = default;

		virtual bool LoadShader() override;
		virtual bool GenerateShader(const char* vertexCode, const char* fragmentCode, const char* geometryCode) override;
		virtual void DeleteShader() override;
		virtual void UseShader() const override;

		virtual void SetInteger(const std::string& name, int value) override;
		virtual void SetUnsignedInteger(const std::string& name, unsigned int value) override;
		virtual void SetIntegerArray(const std::string& name, size_t count, int* values) override;

		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloatArray(const std::string& name, size_t count, float* values) override;

		virtual void SetVector2(const std::string& name, float x, float y) override;
		virtual void SetVector2(const std::string& name, const Vector2& vector) override;

		virtual void SetVector3(const std::string& name, float x, float y, float z) override;
		virtual void SetVector3(const std::string& name, const Vector3& vector) override;

		virtual void SetVector4(const std::string& name, float x, float y, float z, float w) override;
		virtual void SetVector4(const std::string& name, const Vector4& vector) override;

		virtual void SetMatrix2(const std::string& name, const Matrix2& matrix) override;
		virtual void SetMatrix3(const std::string& name, const Matrix3& matrix) override;
		virtual void SetMatrix4(const std::string& name, const Matrix4& matrix) override;

	private:
		bool CheckShaderErrors(uint32_t id, uint32_t shader);
		bool CheckProgramErrors(uint32_t id);
		uint32_t GetUniformLocation(std::string name);
	};
}