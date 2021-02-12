#include "HyperRendering/Material.hpp"

#include "HyperCore/Application.hpp"

namespace HyperRendering
{
	Material::Material(ShaderHandle shader)
		: m_Shader{ shader }
	{
	}

	void Material::Bind()
	{
	}
}
