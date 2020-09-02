#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Hyperion
{
	class Shader
	{
	private:
		unsigned int m_ShaderId;
		std::unordered_map<std::string, unsigned int> m_UniformCache;

	public:
		Shader();
		bool LoadShader(const char* vertexShader, const char* fragmentShader, const char* geometryShader);

		void UseShader();

		void SetFloat(std::string name, float value);
		void SetFloatArray(std::string name, unsigned int count, const float* value);

		void SetInteger(std::string name, int value);
		void SetUnsignedInteger(std::string name, unsigned int value);
		void SetIntegerArray(std::string name, unsigned int count, const int* value);

		void SetVector2(std::string name, float x, float y);
		void SetVector2(std::string name, const glm::vec2& value);

		void SetVector3(std::string name, float x, float y, float z);
		void SetVector3(std::string name, const glm::vec3& value);

		void SetVector4(std::string name, float x, float y, float z, float w);
		void SetVector4(std::string name, const glm::vec4& value);

		void SetMatrix2(std::string name, const glm::mat2& matrix);
		void SetMatrix3(std::string name, const glm::mat3& matrix);
		void SetMatrix4(std::string name, const glm::mat4& matrix);

		unsigned int GetShaderId() const;

	private:
		bool CheckShaderErrors(unsigned int id, unsigned int shader);
		bool CheckProgramErrors(unsigned int id);
		unsigned int GetUniformLocation(std::string name);
	};
}