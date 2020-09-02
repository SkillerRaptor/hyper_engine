#include "Shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Utilities/Log.hpp"

namespace Hyperion
{
	Shader::Shader()
		: m_ShaderId(0), m_UniformCache({})
	{
	}

	bool Shader::LoadShader(const char* vertexShader, const char* fragmentShader, const char* geometryShader)
	{
		std::string vertexTempCode;
		std::string fragmentTempCode;
		std::string geometryTempCode;

		try
		{
			std::ifstream vertexShaderFile(vertexShader);
			std::ifstream fragmentShaderFile(fragmentShader);

			std::stringstream vertexShaderStream, fragmentShaderStream;

			vertexShaderStream << vertexShaderFile.rdbuf();
			fragmentShaderStream << fragmentShaderFile.rdbuf();

			vertexShaderFile.close();
			fragmentShaderFile.close();

			vertexTempCode = vertexShaderStream.str();
			fragmentTempCode = fragmentShaderStream.str();

			if (geometryShader != nullptr)
			{
				std::ifstream     geometryShaderFile(geometryShader);
				std::stringstream geometryShaderStream;

				geometryShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				geometryTempCode = geometryShaderStream.str();
			}
		}
		catch (std::exception exception)
		{
			HP_CORE_ERROR("Shader: Failed to read shader files");
			return false;
		}

		const char* vertexCode = vertexTempCode.c_str();
		const char* fragmentCode = fragmentTempCode.c_str();
		const char* geometryCode = geometryTempCode.c_str();

		unsigned int vertexShaderId = 0;
		unsigned int fragmentShaderId = 0;
		unsigned int geometryShaderId = 0;

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

	void Shader::UseShader()
	{
		glUseProgram(m_ShaderId);
	}

	void Shader::SetFloat(std::string name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetFloatArray(std::string name, unsigned int count, const float* value)
	{
		glUniform1fv(GetUniformLocation(name), count, value);
	}

	void Shader::SetInteger(std::string name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUnsignedInteger(std::string name, unsigned int value)
	{
		glUniform1ui(GetUniformLocation(name), value);
	}

	void Shader::SetIntegerArray(std::string name, unsigned int count, const int* value)
	{
		glUniform1iv(GetUniformLocation(name), count, value);
	}

	void Shader::SetVector2(std::string name, float x, float y)
	{
		glUniform2f(GetUniformLocation(name), x, y);
	}

	void Shader::SetVector2(std::string name, const glm::vec2& value)
	{
		glUniform2f(GetUniformLocation(name), value.x, value.y);
	}

	void Shader::SetVector3(std::string name, float x, float y, float z)
	{
		glUniform3f(GetUniformLocation(name), x, y, z);
	}

	void Shader::SetVector3(std::string name, const glm::vec3& value)
	{
		glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
	}

	void Shader::SetVector4(std::string name, float x, float y, float z, float w)
	{
		glUniform4f(GetUniformLocation(name), x, y, z, w);
	}

	void Shader::SetVector4(std::string name, const glm::vec4& value)
	{
		glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void Shader::SetMatrix2(std::string name, const glm::mat2& matrix)
	{
		glUniformMatrix2fv(GetUniformLocation(name), 1, false, glm::value_ptr(matrix));
	}

	void Shader::SetMatrix3(std::string name, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, false, glm::value_ptr(matrix));
	}

	void Shader::SetMatrix4(std::string name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, false, glm::value_ptr(matrix));
	}

	unsigned int Shader::GetShaderId() const
	{
		return m_ShaderId;
	}

	bool Shader::CheckShaderErrors(unsigned int id, unsigned int shader)
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

	bool Shader::CheckProgramErrors(unsigned int id)
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

	unsigned int Shader::GetUniformLocation(std::string name)
	{
		if (m_UniformCache.find(name) == m_UniformCache.end())
			m_UniformCache[name] = glGetUniformLocation(m_ShaderId, name.c_str());

		return m_UniformCache.at(name);
	}
}