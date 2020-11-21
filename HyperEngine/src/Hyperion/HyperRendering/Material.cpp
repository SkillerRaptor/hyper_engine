#include "Material.hpp"

#include "HyperCore/Application.hpp"

namespace Hyperion
{
	Material::Material(uint32_t shader)
		: m_Shader(shader), m_Texture(0), m_Color(0.0f), m_Ambient(0.0f), m_Diffuse(0.0f), m_Specular(0.0f), m_Shininess(0.0f)
	{
	}

	void Material::Bind()
	{
		Ref<ShaderManager> shaderManager = Application::Get()->GetNativeWindow()->GetContext()->GetShaderManager();
		shaderManager->UseShader(m_Shader);
		shaderManager->SetInteger(m_Shader, "u_Material.Texture", m_Texture);
		shaderManager->SetVector4(m_Shader, "u_Material.Color", m_Color);
		shaderManager->SetVector3(m_Shader, "u_Material.Ambient", m_Ambient);
		shaderManager->SetVector3(m_Shader, "u_Material.Diffuse", m_Diffuse);
		shaderManager->SetVector3(m_Shader, "u_Material.Specular", m_Specular);
		shaderManager->SetFloat(m_Shader, "u_Material.Shininess", m_Shininess);
	}

	void Material::SetColor(Vec4 color)
	{
		m_Color = color;
	}

	const Vec4& Material::GetColor() const
	{
		return m_Color;
	}

	void Material::SetTexture(uint32_t texture)
	{
		m_Texture = texture;
	}

	const uint32_t Material::GetTexture() const
	{
		return m_Texture;
	}

	void Material::SetAmbient(Vec3 ambient)
	{
		m_Ambient = ambient;
	}

	const Vec3& Material::GetAmbient() const
	{
		return m_Ambient;
	}

	void Material::SetDiffuse(Vec3 diffuse)
	{
		m_Diffuse = diffuse;
	}

	const Vec3& Material::GetDiffuse() const
	{
		return m_Diffuse;
	}

	void Material::SetSpecular(Vec3 specular)
	{
		m_Specular = specular;
	}

	const Vec3& Material::GetSpecular() const
	{
		return m_Specular;
	}

	void Material::SetShininess(float shininess)
	{
		m_Shininess = shininess;
	}

	const float Material::GetShininess() const
	{
		return m_Shininess;
	}
}
