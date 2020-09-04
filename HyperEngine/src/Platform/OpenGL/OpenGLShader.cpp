#include "OpenGLShader.hpp"

#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Utilities/Log.hpp"

namespace Hyperion
{
	OpenGLShader::OpenGLShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
		: Shader(vertexShader, fragmentShader, geometryShader)
	{
	}

	bool OpenGLShader::LoadShader()
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;

		try
		{
			std::ifstream vertexShaderFile(m_VertexShaderPath);
			std::ifstream fragmentShaderFile(m_FragmentShaderPath);

			std::stringstream vertexShaderStream, fragmentShaderStream;

			vertexShaderStream << vertexShaderFile.rdbuf();
			fragmentShaderStream << fragmentShaderFile.rdbuf();

			vertexShaderFile.close();
			fragmentShaderFile.close();

			vertexCode = vertexShaderStream.str();
			fragmentCode = fragmentShaderStream.str();

			if (m_GeometryShaderPath != "")
			{
				std::ifstream geometryShaderFile(m_GeometryShaderPath);
				std::stringstream geometryShaderStream;

				geometryShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				geometryCode = geometryShaderStream.str();
			}
		}
		catch (std::exception exception)
		{
			HP_CORE_ERROR("Shader: Failed to read shader files");
			return false;
		}

		return GenerateShader(vertexCode.c_str(), fragmentCode.c_str(), geometryCode != "" ? geometryCode.c_str() : nullptr);
	}

	bool OpenGLShader::GenerateShader(const char* vertexCode, const char* fragmentCode, const char* geometryCode)
	{
		uint32_t vertexShaderId = 0;
		uint32_t fragmentShaderId = 0;
		uint32_t geometryShaderId = 0;

		vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShaderId, 1, &vertexCode, nullptr);
		glCompileShader(vertexShaderId);
		if (!CheckShaderErrors(vertexShaderId, GL_VERTEX_SHADER)) return false;

		fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShaderId, 1, &fragmentCode, nullptr);
		glCompileShader(fragmentShaderId);
		if (!CheckShaderErrors(fragmentShaderId, GL_FRAGMENT_SHADER)) return false;

		if (geometryCode != nullptr)
		{
			geometryShaderId = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShaderId, 1, &geometryCode, nullptr);
			glCompileShader(geometryShaderId);
			if (!CheckShaderErrors(geometryShaderId, GL_GEOMETRY_SHADER)) return false;
		}

		m_ShaderId = glCreateProgram();
		glAttachShader(m_ShaderId, vertexShaderId);
		glAttachShader(m_ShaderId, fragmentShaderId);
		if (geometryCode != nullptr)
			glAttachShader(m_ShaderId, geometryShaderId);

		glLinkProgram(m_ShaderId);
		if (!CheckProgramErrors(m_ShaderId)) return false;

		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
		if (geometryCode != nullptr)
			glDeleteShader(geometryShaderId);
		return true;
	}

	void OpenGLShader::DeleteShader()
	{
		m_UniformCache.clear();
		glDeleteProgram(m_ShaderId);
	}

	void OpenGLShader::UseShader() const
	{
		glUseProgram(m_ShaderId);
	}

	void OpenGLShader::SetInteger(const std::string& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUnsignedInteger(const std::string& name, unsigned int value)
	{
		glUniform1ui(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetIntegerArray(const std::string& name, size_t count, int* values)
	{
		glUniform1iv(GetUniformLocation(name), static_cast<GLsizei>(count), values);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetFloatArray(const std::string& name, size_t count, float* values)
	{
		glUniform1fv(GetUniformLocation(name), static_cast<GLsizei>(count), values);
	}

	void OpenGLShader::SetVector2(const std::string& name, float x, float y)
	{
		glUniform2f(GetUniformLocation(name), x, y);
	}

	void OpenGLShader::SetVector2(const std::string& name, const Vector2& vector)
	{
		glUniform2f(GetUniformLocation(name), vector.x, vector.y);
	}

	void OpenGLShader::SetVector3(const std::string& name, float x, float y, float z)
	{
		glUniform3f(GetUniformLocation(name), x, y, z);
	}

	void OpenGLShader::SetVector3(const std::string& name, const Vector3& vector)
	{
		glUniform3f(GetUniformLocation(name), vector.x, vector.y, vector.z);
	}

	void OpenGLShader::SetVector4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(GetUniformLocation(name), x, y, z, w);
	}

	void OpenGLShader::SetVector4(const std::string& name, const Vector4& vector)
	{
		glUniform4f(GetUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
	}

	void OpenGLShader::SetMatrix2(const std::string& name, const Matrix2& matrix)
	{
		glUniformMatrix2fv(GetUniformLocation(name), 1, false, &matrix.matrix[0][0]);
	}

	void OpenGLShader::SetMatrix3(const std::string& name, const Matrix3& matrix)
	{
		glUniformMatrix2fv(GetUniformLocation(name), 1, false, &matrix.matrix[0][0]);
	}

	void OpenGLShader::SetMatrix4(const std::string& name, const Matrix4& matrix)
	{
		glUniformMatrix2fv(GetUniformLocation(name), 1, false, &matrix.matrix[0][0]);
	}

	bool OpenGLShader::CheckShaderErrors(uint32_t id, uint32_t shader)
	{
		int success;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (success)
			return true;

		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(sizeof(char) * length);
		glGetShaderInfoLog(id, length, &length, message);

		const char* shaderType;

		switch (shader)
		{
		case GL_VERTEX_SHADER:
			shaderType = "Vertex";
			break;
		case GL_FRAGMENT_SHADER:
			shaderType = "Fragment";
			break;
		case GL_GEOMETRY_SHADER:
			shaderType = "Geometry";
			break;
		default:
			shaderType = "Not Found!";
			break;
		}

		HP_CORE_ERROR("Shader - Compile-time error: % | %", shaderType, message);
		return false;
	}

	bool OpenGLShader::CheckProgramErrors(uint32_t id)
	{
		int success;
		glGetProgramiv(id, GL_LINK_STATUS, &success);

		if (success)
			return true;

		int length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(sizeof(char) * length);
		glGetProgramInfoLog(id, length, &length, message);

		HP_CORE_ERROR("Shader - Link-time error: Program | %", message);
		return false;
	}

	uint32_t OpenGLShader::GetUniformLocation(std::string name)
	{
		if (m_UniformCache.find(name) == m_UniformCache.end())
			m_UniformCache[name] = glGetUniformLocation(m_ShaderId, name.c_str());

		return m_UniformCache.at(name);
	}
}