#include "OpenGLShaderManager.hpp"

#include <fstream>

#include "Utilities/Log.hpp"

namespace Hyperion
{
	OpenGLShaderManager::OpenGLShaderManager()
	{
		HP_CORE_DEBUG("OpenGL Shader-Manager loaded...");
	}

	OpenGLShaderManager::~OpenGLShaderManager()
	{
		HP_CORE_DEBUG("OpenGL Shader-Manager unloaded...");
		for (const auto& [handle, shaderData] : m_Shaders)
			glDeleteProgram(shaderData.ShaderId);
		m_Shaders.clear();
	}

	uint32_t OpenGLShaderManager::CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
	{
		OpenGLShaderData shaderData;
		shaderData.VertexShaderPath = vertexPath;
		shaderData.FragmentShaderPath = fragmentPath;
		shaderData.GeometryShaderPath = geometryPath;

		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;

		try
		{
			std::ifstream vertexShaderFile(shaderData.VertexShaderPath);
			std::ifstream fragmentShaderFile(shaderData.FragmentShaderPath);

			std::stringstream vertexShaderStream, fragmentShaderStream;

			vertexShaderStream << vertexShaderFile.rdbuf();
			fragmentShaderStream << fragmentShaderFile.rdbuf();

			vertexShaderFile.close();
			fragmentShaderFile.close();

			vertexCode = vertexShaderStream.str();
			fragmentCode = fragmentShaderStream.str();

			if (shaderData.GeometryShaderPath != "")
			{
				std::ifstream geometryShaderFile(shaderData.GeometryShaderPath);
				std::stringstream geometryShaderStream;

				geometryShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				geometryCode = geometryShaderStream.str();
			}
		}
		catch (std::exception exception)
		{
			HP_CORE_ERROR("Shader: Failed to read shader files");
			return -1;
		}

		if (!GenerateShader(shaderData, vertexCode.c_str(), fragmentCode.c_str(), geometryCode != "" ? geometryCode.c_str() : nullptr))
		{
			HP_CORE_DEBUG("Shader not loaded...");
			return -1;
		}

		uint32_t shaderId = 0;
		if (!m_ShaderIds.empty())
		{
			shaderId = m_ShaderIds.front();
			m_ShaderIds.pop();
		}
		else
			shaderId = static_cast<uint32_t>(m_Shaders.size());
		m_Shaders.emplace(shaderId, std::move(shaderData));
		HP_CORE_DEBUG("Shader % loaded...", shaderId);
		return shaderId;
	}

	bool OpenGLShaderManager::GenerateShader(OpenGLShaderData& shaderData, const char* vertexCode, const char* fragmentCode, const char* geometryCode)
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

		shaderData.ShaderId = glCreateProgram();
		glAttachShader(shaderData.ShaderId, vertexShaderId);
		glAttachShader(shaderData.ShaderId, fragmentShaderId);
		if (geometryCode != nullptr)
			glAttachShader(shaderData.ShaderId, geometryShaderId);

		glLinkProgram(shaderData.ShaderId);
		if (!CheckProgramErrors(shaderData)) return false;

		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
		if (geometryCode != nullptr)
			glDeleteShader(geometryShaderId);
		return true;
	}

	bool OpenGLShaderManager::UseShader(uint32_t handle)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return false;
		glUseProgram(m_Shaders[handle].ShaderId);
		return true;
	}

	bool OpenGLShaderManager::DeleteShader(uint32_t handle)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return false;
		m_Shaders[handle].UniformCache.clear();
		glDeleteProgram(m_Shaders[handle].ShaderId);
		m_Shaders.erase(handle);
		m_ShaderIds.push(handle);
		return true;
	}

	void OpenGLShaderManager::SetInteger(uint32_t handle, const std::string& name, int value)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform1i(GetUniformLocation(m_Shaders[handle], name), value);
	}

	void OpenGLShaderManager::SetUnsignedInteger(uint32_t handle, const std::string& name, unsigned int value)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform1ui(GetUniformLocation(m_Shaders[handle], name), value);
	}

	void OpenGLShaderManager::SetIntegerArray(uint32_t handle, const std::string& name, size_t count, int* values)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform1iv(GetUniformLocation(m_Shaders[handle], name), static_cast<GLsizei>(count), values);
	}

	void OpenGLShaderManager::SetFloat(uint32_t handle, const std::string& name, float value)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform1f(GetUniformLocation(m_Shaders[handle], name), value);
	}

	void OpenGLShaderManager::SetFloatArray(uint32_t handle, const std::string& name, size_t count, float* values)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform1fv(GetUniformLocation(m_Shaders[handle], name), static_cast<GLsizei>(count), values);
	}

	void OpenGLShaderManager::SetVector2(uint32_t handle, const std::string& name, float x, float y)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform2f(GetUniformLocation(m_Shaders[handle], name), x, y);
	}

	void OpenGLShaderManager::SetVector2(uint32_t handle, const std::string& name, const Vector2<>& vector)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform2f(GetUniformLocation(m_Shaders[handle], name), vector.x, vector.y);
	}

	void OpenGLShaderManager::SetVector3(uint32_t handle, const std::string& name, float x, float y, float z)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform3f(GetUniformLocation(m_Shaders[handle], name), x, y, z);
	}

	void OpenGLShaderManager::SetVector3(uint32_t handle, const std::string& name, const Vector3<>& vector)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform3f(GetUniformLocation(m_Shaders[handle], name), vector.x, vector.y, vector.z);
	}

	void OpenGLShaderManager::SetVector4(uint32_t handle, const std::string& name, float x, float y, float z, float w)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform4f(GetUniformLocation(m_Shaders[handle], name), x, y, z, w);
	}

	void OpenGLShaderManager::SetVector4(uint32_t handle, const std::string& name, const Vector4<>& vector)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniform4f(GetUniformLocation(m_Shaders[handle], name), vector.x, vector.y, vector.z, vector.w);
	}

	void OpenGLShaderManager::SetMatrix2(uint32_t handle, const std::string& name, const Matrix2<>& matrix)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniformMatrix2fv(GetUniformLocation(m_Shaders[handle], name), 1, false, &matrix.matrix[0][0]);
	}

	void OpenGLShaderManager::SetMatrix3(uint32_t handle, const std::string& name, const Matrix3<>& matrix)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniformMatrix3fv(GetUniformLocation(m_Shaders[handle], name), 1, false, &matrix.matrix[0][0]);
	}

	void OpenGLShaderManager::SetMatrix4(uint32_t handle, const std::string& name, const Matrix4<>& matrix)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniformMatrix4fv(GetUniformLocation(m_Shaders[handle], name), 1, false, &matrix.matrix[0][0]);
	}

	std::string OpenGLShaderManager::GetVertexShaderPath(uint32_t handle)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return "";
		return m_Shaders[handle].VertexShaderPath;
	}

	std::string OpenGLShaderManager::GetFragmentShaderPath(uint32_t handle)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return "";
		return m_Shaders[handle].FragmentShaderPath;
	}

	std::string OpenGLShaderManager::GetGeometryShaderPath(uint32_t handle)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return "";
		return m_Shaders[handle].GeometryShaderPath;
	}

	bool OpenGLShaderManager::CheckShaderErrors(uint32_t id, uint32_t shader)
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

	bool OpenGLShaderManager::CheckProgramErrors(OpenGLShaderData& shaderData)
	{
		int success;
		glGetProgramiv(shaderData.ShaderId, GL_LINK_STATUS, &success);

		if (success)
			return true;

		int length;
		glGetProgramiv(shaderData.ShaderId, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(sizeof(char) * length);
		glGetProgramInfoLog(shaderData.ShaderId, length, &length, message);

		HP_CORE_ERROR("Shader - Link-time error: Program | %", message);
		return false;
	}

	uint32_t OpenGLShaderManager::GetUniformLocation(OpenGLShaderData& shaderData, std::string name)
	{
		if (shaderData.UniformCache.find(name) == shaderData.UniformCache.end())
			shaderData.UniformCache[name] = glGetUniformLocation(shaderData.ShaderId, name.c_str());

		return shaderData.UniformCache.at(name);
	}
}
