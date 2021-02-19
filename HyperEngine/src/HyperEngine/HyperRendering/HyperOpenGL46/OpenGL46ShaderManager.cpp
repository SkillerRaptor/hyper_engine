#include "HyperRendering/HyperOpenGL46/OpenGL46ShaderManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include "HyperUtilities/FileUtilities.hpp"
#include "HyperUtilities/Log.hpp"

namespace HyperRendering
{
	OpenGL46ShaderManager::OpenGL46ShaderManager()
	{
		HP_CORE_DEBUG("OpenGL Shader-Manager loaded...");
	}

	OpenGL46ShaderManager::~OpenGL46ShaderManager()
	{
		HP_CORE_DEBUG("OpenGL Shader-Manager unloaded...");
		for (const auto& [handle, shaderData] : m_Shaders)
			glDeleteProgram(shaderData.ShaderId);
		m_Shaders.clear();
	}

	ShaderHandle OpenGL46ShaderManager::CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
	{
		OpenGLShaderData shaderData;
		shaderData.VertexShaderPath = vertexPath.c_str();
		shaderData.FragmentShaderPath = fragmentPath.c_str();
		shaderData.GeometryShaderPath = geometryPath.c_str();

		std::string vertexCode = HyperUtilities::FileUtilities::ReadFileContent(vertexPath);
		std::string fragmentCode = HyperUtilities::FileUtilities::ReadFileContent(fragmentPath);
		std::string geometryCode = !geometryPath.empty() ? HyperUtilities::FileUtilities::ReadFileContent(geometryPath) : "";

		if (!GenerateShader(shaderData, vertexCode.c_str(), fragmentCode.c_str(), !geometryPath.empty() ? geometryCode.c_str() : nullptr))
		{
			HP_CORE_DEBUG("Shader not loaded...");
			return { static_cast<uint32_t>(-1) };
		}

		ShaderHandle shaderId = { 1 };
		if (!m_ShaderIds.empty())
		{
			shaderId = m_ShaderIds.front();
			m_ShaderIds.pop();
		}
		else
			shaderId = static_cast<ShaderHandle>(ShaderHandle{ static_cast<uint32_t>(m_Shaders.size()) + 1 });
		m_Shaders.emplace(shaderId, std::move(shaderData));
		return shaderId;
	}

	bool OpenGL46ShaderManager::GenerateShader(OpenGLShaderData& shaderData, const char* vertexCode, const char* fragmentCode, const char* geometryCode)
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

	void OpenGL46ShaderManager::UseShader(ShaderHandle handle)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUseProgram(m_Shaders[handle].ShaderId);
	}

	void OpenGL46ShaderManager::DeleteShader(ShaderHandle handle)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		m_Shaders[handle].UniformCache.clear();
		glDeleteProgram(m_Shaders[handle].ShaderId);
		m_Shaders.erase(handle);
		m_ShaderIds.push(handle);
	}

	void OpenGL46ShaderManager::SetInteger(ShaderHandle handle, const std::string& name, int value)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform1i(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), value);
	}

	void OpenGL46ShaderManager::SetUnsignedInteger(ShaderHandle handle, const std::string& name, unsigned int value)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform1ui(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), value);
	}

	void OpenGL46ShaderManager::SetIntegerArray(ShaderHandle handle, const std::string& name, size_t count, int* values)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform1iv(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), static_cast<GLsizei>(count), values);
	}

	void OpenGL46ShaderManager::SetFloat(ShaderHandle handle, const std::string& name, float value)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform1f(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), value);
	}

	void OpenGL46ShaderManager::SetFloatArray(ShaderHandle handle, const std::string& name, size_t count, float* values)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform1fv(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), static_cast<GLsizei>(count), values);
	}

	void OpenGL46ShaderManager::SetVector2(ShaderHandle handle, const std::string& name, float x, float y)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform2f(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), x, y);
	}

	void OpenGL46ShaderManager::SetVector2(ShaderHandle handle, const std::string& name, const glm::vec2& vector)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform2f(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), vector.x, vector.y);
	}

	void OpenGL46ShaderManager::SetVector3(ShaderHandle handle, const std::string& name, float x, float y, float z)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform3f(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), x, y, z);
	}

	void OpenGL46ShaderManager::SetVector3(ShaderHandle handle, const std::string& name, const glm::vec3& vector)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform3f(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), vector.x, vector.y, vector.z);
	}

	void OpenGL46ShaderManager::SetVector4(ShaderHandle handle, const std::string& name, float x, float y, float z, float w)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform4f(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), x, y, z, w);
	}

	void OpenGL46ShaderManager::SetVector4(ShaderHandle handle, const std::string& name, const glm::vec4& vector)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glProgramUniform4f(m_Shaders[handle].ShaderId, GetUniformLocation(m_Shaders[handle], name), vector.x, vector.y, vector.z, vector.w);
	}

	void OpenGL46ShaderManager::SetMatrix2(ShaderHandle handle, const std::string& name, const glm::mat2& matrix)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniformMatrix2fv(GetUniformLocation(m_Shaders[handle], name), 1, false, glm::value_ptr(matrix));
	}

	void OpenGL46ShaderManager::SetMatrix3(ShaderHandle handle, const std::string& name, const glm::mat3& matrix)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniformMatrix3fv(GetUniformLocation(m_Shaders[handle], name), 1, false, glm::value_ptr(matrix));
	}

	void OpenGL46ShaderManager::SetMatrix4(ShaderHandle handle, const std::string& name, const glm::mat4& matrix)
	{
		if (m_Shaders.find(handle) == m_Shaders.end())
			return;
		glUniformMatrix4fv(GetUniformLocation(m_Shaders[handle], name), 1, false, glm::value_ptr(matrix));
	}

	bool OpenGL46ShaderManager::CheckShaderErrors(uint32_t id, uint32_t shader)
	{
		int success;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (success)
			return true;

		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
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

		HP_CORE_ERROR("Shader - Compile-time error: {} | {}", shaderType, message);
		delete[] message;
		return false;
	}

	bool OpenGL46ShaderManager::CheckProgramErrors(OpenGLShaderData& shaderData)
	{
		int success;
		glGetProgramiv(shaderData.ShaderId, GL_LINK_STATUS, &success);

		if (success)
			return true;

		int length;
		glGetProgramiv(shaderData.ShaderId, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetProgramInfoLog(shaderData.ShaderId, length, &length, message);

		HP_CORE_ERROR("Shader - Link-time error: Program | {}", message);
		delete[] message;
		return false;
	}

	uint32_t OpenGL46ShaderManager::GetUniformLocation(OpenGLShaderData& shaderData, const std::string& name)
	{
		if (shaderData.UniformCache.find(name) == shaderData.UniformCache.end())
			shaderData.UniformCache[name] = glGetUniformLocation(shaderData.ShaderId, name.c_str());

		return shaderData.UniformCache.at(name);
	}
}