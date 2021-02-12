#pragma once

#include <glm/glm.hpp>

#include "HyperRendering/TextureManager.hpp"

namespace HyperECS
{
	class Rigidbody2DComponent
	{
	private:
		float m_Mass;

	public:
		Rigidbody2DComponent() = default;
		~Rigidbody2DComponent() = default;

		Rigidbody2DComponent(float mass);

		Rigidbody2DComponent(const Rigidbody2DComponent& other);
		Rigidbody2DComponent& operator=(const Rigidbody2DComponent& other);

		Rigidbody2DComponent(Rigidbody2DComponent&& other) noexcept;
		Rigidbody2DComponent& operator=(Rigidbody2DComponent&& other) noexcept;

		bool operator==(const Rigidbody2DComponent& other) const;
		bool operator!=(const Rigidbody2DComponent& other) const;

		inline Rigidbody2DComponent& SetMass(float mass)
		{
			m_Mass = mass;
			return *this;
		}

		inline float GetMass() const
		{
			return m_Mass;
		}
	};
}
