#pragma once

#include "HyperMath/Vector.hpp"

namespace Hyperion
{
	class Material
	{
	private:
		uint32_t m_Shader;

	public:
		Material(uint32_t shader);
		~Material();

		void Bind();

		template<typename T>
		void Set(const std::string& name, const T& value)
		{

		}
	};
}
