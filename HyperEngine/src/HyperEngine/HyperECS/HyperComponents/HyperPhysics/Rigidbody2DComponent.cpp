#include "HyperECS/HyperComponents/HyperPhysics/Rigidbody2DComponent.hpp"

#include "HyperUtilities/Reflection.hpp"

namespace HyperECS
{
	Rigidbody2DComponent::Rigidbody2DComponent(float mass)
		: m_Mass{ mass }
	{
	}

	Rigidbody2DComponent::Rigidbody2DComponent(const Rigidbody2DComponent& other)
		: m_Mass{ other.m_Mass }
	{
	}

	Rigidbody2DComponent& Rigidbody2DComponent::operator=(const Rigidbody2DComponent& other)
	{
		m_Mass = other.m_Mass;
		return *this;
	}

	Rigidbody2DComponent::Rigidbody2DComponent(Rigidbody2DComponent&& other) noexcept
		: m_Mass{ std::move(other.m_Mass) }
	{
	}

	Rigidbody2DComponent& Rigidbody2DComponent::operator=(Rigidbody2DComponent&& other) noexcept
	{
		m_Mass = std::move(other.m_Mass);
		return *this;
	}

	bool Rigidbody2DComponent::operator==(const Rigidbody2DComponent& other) const
	{
		return (m_Mass == other.m_Mass);
	}

	bool Rigidbody2DComponent::operator!=(const Rigidbody2DComponent& other) const
	{
		return !(*this == other);
	}

	HP_REFLECT_TYPE
	{
		using namespace HyperUtilities;

		rttr::registration::class_<Rigidbody2DComponent>("Rigidbody Component")
			(
				rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<Rigidbody2DComponent>),
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.constructor<>()
			.constructor<float>()
			.property("Mass", &Rigidbody2DComponent::GetMass, &Rigidbody2DComponent::SetMass)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			);
	}
}
