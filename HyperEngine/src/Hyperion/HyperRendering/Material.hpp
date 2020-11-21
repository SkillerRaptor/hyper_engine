#pragma once

#include "HyperMath/Vector.hpp"

namespace Hyperion
{
	class Material
	{
	private:
		uint32_t m_Shader;
		uint32_t m_Texture;
		Vec4 m_Color;
		Vec3 m_Ambient;
		Vec3 m_Diffuse;
		Vec3 m_Specular;
		float m_Shininess;

	public:
		Material(uint32_t shader);

		void Bind();

		void SetColor(Vec4 color);
		const Vec4& GetColor() const;

		void SetTexture(uint32_t texture);
		const uint32_t GetTexture() const;

		void SetAmbient(Vec3 ambient);
		const Vec3& GetAmbient() const;

		void SetDiffuse(Vec3 diffuse);
		const Vec3& GetDiffuse() const;

		void SetSpecular(Vec3 specular);
		const Vec3& GetSpecular() const;

		void SetShininess(float shininess);
		const float GetShininess() const;
	};
}
