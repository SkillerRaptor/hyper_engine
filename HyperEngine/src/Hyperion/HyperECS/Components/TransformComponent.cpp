#include "TransformComponent.hpp"

#include "HyperUtilities/Reflection.hpp"

namespace Hyperion
{
	TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
		: m_Position(position), m_Rotation(rotation), m_Scale(scale)
	{
	}

	TransformComponent::TransformComponent(const TransformComponent& other)
		: m_Position(other.m_Position), m_Rotation(other.m_Rotation), m_Scale(other.m_Scale)
	{
	}

	TransformComponent& TransformComponent::operator=(const TransformComponent& other)
	{
		m_Position = other.m_Position;
		m_Rotation = other.m_Rotation;
		m_Scale = other.m_Scale;
		return *this;
	}

	TransformComponent::TransformComponent(TransformComponent&& other) noexcept
		: m_Position(std::move(other.m_Position)), m_Rotation(std::move(other.m_Rotation)), m_Scale(std::move(other.m_Scale))
	{
	}

	TransformComponent& TransformComponent::operator=(TransformComponent&& other) noexcept
	{
		m_Position = std::move(other.m_Position);
		m_Rotation = std::move(other.m_Rotation);
		m_Scale = std::move(other.m_Scale);
		return *this;
	}

	bool TransformComponent::operator==(const TransformComponent& other) const
	{
		return (m_Position == other.m_Position && m_Rotation == other.m_Rotation && m_Scale == other.m_Scale);
	}

	bool TransformComponent::operator!=(const TransformComponent& other) const
	{
		return !(*this == other);
	}

	TransformComponent& TransformComponent::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		return *this;
	}

	const glm::vec3& TransformComponent::GetPosition() const
	{
		return m_Position;
	}

	TransformComponent& TransformComponent::SetRotation(const glm::vec3& rotation)
	{
		m_Rotation = rotation;
		return *this;
	}

	const glm::vec3& TransformComponent::GetRotation() const
	{
		return m_Rotation;
	}

	TransformComponent& TransformComponent::SetScale(const glm::vec3& scale)
	{
		m_Scale = scale;
		return *this;
	}

	const glm::vec3& TransformComponent::GetScale() const
	{
		return m_Scale;
	}

	HP_REFLECT_TYPE
	{
		rttr::registration::class_<TransformComponent>("Transform Component")
			(
				rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<TransformComponent>),
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.constructor<>()
			.constructor<const glm::vec3&, const glm::vec3&, const glm::vec3&>()
			.property("Position", &TransformComponent::GetPosition, &TransformComponent::SetPosition)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
				rttr::metadata(EditorInfo::EDIT_PRECISION, 0.1f)
			)
			.property("Rotation", &TransformComponent::GetRotation, &TransformComponent::SetRotation)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.property("Scale", &TransformComponent::GetScale, &TransformComponent::SetScale)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			);
	}
}
