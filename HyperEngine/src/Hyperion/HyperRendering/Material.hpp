#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

namespace Hyperion
{
	class Material
	{
	private:
		uint32_t m_Shader;

	public:
		explicit Material(uint32_t shader);
		~Material();

		void Bind();

		template<typename T>
		void Set(const std::string& name, const T& value)
		{

		}
	};
}
