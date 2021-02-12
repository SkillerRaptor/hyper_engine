#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "HyperRendering/ShaderManager.hpp"

namespace HyperRendering
{
	class Material
	{
	private:
		ShaderHandle m_Shader;

	public:
		explicit Material(ShaderHandle shader);
		~Material() = default;

		void Bind();

		template<typename T>
		void Set(const std::string& name, const T& value)
		{

		}
	};
}
