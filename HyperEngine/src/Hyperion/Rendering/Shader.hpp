#pragma once

#include <string>

#include "Maths/Matrix.hpp"
#include "Maths/Vector.hpp"

namespace Hyperion
{
	class Shader
	{
	protected:
		std::string m_VertexShaderPath = "";
		std::string m_FragmentShaderPath = "";
		std::string m_GeometryShaderPath = "";

	public:
		Shader() = default;
		Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
			: m_VertexShaderPath(vertexShader), m_FragmentShaderPath(fragmentShader), m_GeometryShaderPath(geometryShader) {}
		
		virtual ~Shader() = default;

		virtual bool LoadShader() = 0;
		virtual bool GenerateShader(const char* vertexCode, const char* fragmentCode, const char* geometryCode) = 0;
		virtual void DeleteShader() = 0;
		virtual void UseShader() const = 0;

		virtual void SetInteger(const std::string& name, int value) = 0;
		virtual void SetUnsignedInteger(const std::string& name, unsigned int value) = 0;
		virtual void SetIntegerArray(const std::string& name, size_t count, int* values) = 0;

		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloatArray(const std::string& name, size_t count, float* values) = 0;

		virtual void SetVector2(const std::string& name, float x, float y) = 0;
		virtual void SetVector2(const std::string& name, const Vector2& vector) = 0;

		virtual void SetVector3(const std::string& name, float x, float y, float z) = 0;
		virtual void SetVector3(const std::string& name, const Vector3& vector) = 0;

		virtual void SetVector4(const std::string& name, float x, float y, float z, float w) = 0;
		virtual void SetVector4(const std::string& name, const Vector4& vector) = 0;

		virtual void SetMatrix2(const std::string& name, const Matrix2& matrix) = 0;
		virtual void SetMatrix3(const std::string& name, const Matrix3& matrix) = 0;
		virtual void SetMatrix4(const std::string& name, const Matrix4& matrix) = 0;

		virtual std::string GetVertexShaderPath() const { return m_VertexShaderPath; }
		virtual std::string GetFragmentShaderPath() const { return m_FragmentShaderPath; }
		virtual std::string GetGeometryShaderPath() const { return m_GeometryShaderPath; }
	};
}