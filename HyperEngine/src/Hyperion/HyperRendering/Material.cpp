#include "Material.hpp"

#include "HyperCore/Application.hpp"

namespace Hyperion
{
	Material::Material(uint32_t shader)
		: m_Shader(shader)
	{
	}

	Material::~Material()
	{

	}

	void Material::Bind()
	{
		Ref<ShaderManager> shaderManager = Application::Get()->GetNativeWindow()->GetContext()->GetShaderManager();

	}
}
