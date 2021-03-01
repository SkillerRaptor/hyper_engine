#include "HyperRendering/HyperOpenGL46/OpenGL46ShaderManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

namespace HyperRendering
{
	OpenGL46ShaderManager::OpenGL46ShaderManager()
		: m_ShaderStorage{ 256 }
	{
		HP_CORE_DEBUG("OpenGL 4.6 Shader-Manager loaded...");
	}

	OpenGL46ShaderManager::~OpenGL46ShaderManager()
	{
		for (size_t i = 0; i < m_ShaderStorage.GetPoolSize(); i++)
			glDeleteTextures(1, &m_ShaderStorage[i].Id);
		m_ShaderStorage.Clear();
		
		HP_CORE_DEBUG("OpenGL 4.6 Shader-Manager unloaded...");
	}

	ShaderHandle OpenGL46ShaderManager::Create(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
	{
		std::string vertexStringCode;
		const char* vertexCode;
		std::ifstream vertexShaderFile;
		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vertexShaderFile.open(vertexPath);
			
			std::stringstream vertexShaderStream;
			vertexShaderStream << vertexShaderFile.rdbuf();
			vertexShaderFile.close();
			
			vertexStringCode = vertexShaderStream.str();
			vertexCode = vertexStringCode.c_str();
		}
		catch (std::exception& ex)
		{
			HP_CORE_ERROR("Failed to read vertex shader: {}!", ex.what());
			return ShaderHandle{ 0 };
		}
		
		std::string fragmentStringCode;
		const char* fragmentCode;
		std::ifstream fragmentShaderFile;
		fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			fragmentShaderFile.open(fragmentPath);
			
			std::stringstream fragmentShaderStream;
			fragmentShaderStream << fragmentShaderFile.rdbuf();
			fragmentShaderFile.close();
			
			fragmentStringCode = fragmentShaderStream.str();
			fragmentCode = fragmentStringCode.c_str();
		}
		catch (std::exception& ex)
		{
			HP_CORE_ERROR("Failed to read fragment shader: {}!", ex.what());
			return ShaderHandle{ 0 };
		}
		
		std::string geometryStringCode;
		const char* geometryCode;
		if (!geometryPath.empty())
		{
			std::ifstream geometryShaderFile;
			geometryShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				geometryShaderFile.open(geometryPath);
				
				std::stringstream geometryShaderStream;
				geometryShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				
				geometryStringCode = geometryShaderStream.str();
				geometryCode = geometryStringCode.c_str();
			}
			catch (std::exception& ex)
			{
				HP_CORE_ERROR("Failed to read geometry shader: {}!", ex.what());
				return ShaderHandle{ 0 };
			}
		}
		
		uint32_t vertexShaderId{ 0 };
		uint32_t fragmentShaderId{ 0 };
		uint32_t geometryShaderId{ 0 };
		
		uint32_t id = glCreateProgram();
		glAttachShader(id, vertexShaderId);
		glAttachShader(id, fragmentShaderId);
		if (!geometryPath.empty())
			glAttachShader(id, geometryShaderId);
		
		vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShaderId, 1, &vertexCode, nullptr);
		glCompileShader(vertexShaderId);
		if (!CheckShaderErrors(vertexShaderId, GL_VERTEX_SHADER))
		{
			HP_CORE_ERROR("Failed to compile vertex shader!");
			return ShaderHandle{ 0 };
		}
		
		fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShaderId, 1, &fragmentCode, nullptr);
		glCompileShader(fragmentShaderId);
		if (!CheckShaderErrors(fragmentShaderId, GL_FRAGMENT_SHADER))
		{
			HP_CORE_ERROR("Failed to compile fragment shader!");
			return ShaderHandle{ 0 };
		}
		
		if (!geometryPath.empty())
		{
			geometryShaderId = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShaderId, 1, &geometryCode, nullptr);
			glCompileShader(geometryShaderId);
			if (!CheckShaderErrors(geometryShaderId, GL_GEOMETRY_SHADER))
			{
				HP_CORE_ERROR("Failed to compile geometry shader!");
				return ShaderHandle{ 0 };
			}
		}
		
		glLinkProgram(id);
		glAttachShader(id, vertexShaderId);
		glAttachShader(id, fragmentShaderId);
		if (!geometryPath.empty())
			glAttachShader(id, geometryShaderId);
		glLinkProgram(id);
		if (!CheckProgramErrors(id))
		{
			HP_CORE_ERROR("Failed to link shader!");
			return ShaderHandle{ 0 };
		}
		
		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
		if (!geometryPath.empty())
			glDeleteShader(geometryShaderId);
		
		uint32_t index{};
		ShaderData& shaderData = m_ShaderStorage.Allocate(index);
		shaderData.MagicNumber = m_ShaderVersion++;
		shaderData.Id = id;
		shaderData.UniformCache = {};
		
		return ShaderHandle{ (shaderData.MagicNumber << 16) | index };
	}

	void OpenGL46ShaderManager::Use(ShaderHandle handle)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glUseProgram(shaderData.Id);
	}

	void OpenGL46ShaderManager::Delete(ShaderHandle handle)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		shaderData.UniformCache.clear();
		glDeleteProgram(shaderData.Id);
		m_ShaderStorage.Deallocate(handle.GetIndex());
	}

	void OpenGL46ShaderManager::SetInteger(ShaderHandle handle, const std::string& name, int value)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform1i(shaderData.Id, GetUniformLocation(shaderData, name), value);
	}

	void OpenGL46ShaderManager::SetUnsignedInteger(ShaderHandle handle, const std::string& name, unsigned int value)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform1ui(shaderData.Id, GetUniformLocation(shaderData, name), value);
	}

	void OpenGL46ShaderManager::SetIntegerArray(ShaderHandle handle, const std::string& name, size_t count, int* values)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform1iv(shaderData.Id, GetUniformLocation(shaderData, name), static_cast<GLsizei>(count), values);
	}

	void OpenGL46ShaderManager::SetFloat(ShaderHandle handle, const std::string& name, float value)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform1f(shaderData.Id, GetUniformLocation(shaderData, name), value);
	}

	void OpenGL46ShaderManager::SetFloatArray(ShaderHandle handle, const std::string& name, size_t count, float* values)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform1fv(shaderData.Id, GetUniformLocation(shaderData, name), static_cast<GLsizei>(count), values);
	}

	void OpenGL46ShaderManager::SetVector2(ShaderHandle handle, const std::string& name, float x, float y)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform2f(shaderData.Id, GetUniformLocation(shaderData, name), x, y);
	}

	void OpenGL46ShaderManager::SetVector2(ShaderHandle handle, const std::string& name, const glm::vec2& vector)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform2f(shaderData.Id, GetUniformLocation(shaderData, name), vector.x, vector.y);
	}

	void OpenGL46ShaderManager::SetVector3(ShaderHandle handle, const std::string& name, float x, float y, float z)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform3f(shaderData.Id, GetUniformLocation(shaderData, name), x, y, z);
	}

	void OpenGL46ShaderManager::SetVector3(ShaderHandle handle, const std::string& name, const glm::vec3& vector)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform3f(shaderData.Id, GetUniformLocation(shaderData, name), vector.x, vector.y, vector.z);
	}

	void OpenGL46ShaderManager::SetVector4(ShaderHandle handle, const std::string& name, float x, float y, float z, float w)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform4f(shaderData.Id, GetUniformLocation(shaderData, name), x, y, z, w);
	}

	void OpenGL46ShaderManager::SetVector4(ShaderHandle handle, const std::string& name, const glm::vec4& vector)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glProgramUniform4f(shaderData.Id, GetUniformLocation(shaderData, name), vector.x, vector.y, vector.z, vector.w);
	}

	void OpenGL46ShaderManager::SetMatrix2(ShaderHandle handle, const std::string& name, const glm::mat2& matrix)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glUniformMatrix2fv(GetUniformLocation(shaderData, name), 1, false, glm::value_ptr(matrix));
	}

	void OpenGL46ShaderManager::SetMatrix3(ShaderHandle handle, const std::string& name, const glm::mat3& matrix)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glUniformMatrix3fv(GetUniformLocation(shaderData, name), 1, false, glm::value_ptr(matrix));
	}

	void OpenGL46ShaderManager::SetMatrix4(ShaderHandle handle, const std::string& name, const glm::mat4& matrix)
	{
		ShaderData& shaderData = m_ShaderStorage[handle.GetIndex()];
		if (shaderData.MagicNumber != handle.GetVersion())
			return;
		glUniformMatrix4fv(GetUniformLocation(shaderData, name), 1, false, glm::value_ptr(matrix));
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

	bool OpenGL46ShaderManager::CheckProgramErrors(uint32_t id)
	{
		int success;
		glGetProgramiv(id, GL_LINK_STATUS, &success);

		if (success)
			return true;

		int length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetProgramInfoLog(id, length, &length, message);

		HP_CORE_ERROR("Shader - Link-time error: Program | {}", message);
		delete[] message;
		return false;
	}

	uint32_t OpenGL46ShaderManager::GetUniformLocation(ShaderData& shaderData, const std::string& name)
	{
		if (shaderData.UniformCache.find(name) == shaderData.UniformCache.end())
			shaderData.UniformCache[name] = glGetUniformLocation(shaderData.Id, name.c_str());

		return shaderData.UniformCache.at(name);
	}
}
