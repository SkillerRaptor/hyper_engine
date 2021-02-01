#include "SpriteRendererComponent.hpp"

#include "HyperUtilities/Reflection.hpp"

namespace Hyperion
{
	SpriteRendererComponent::SpriteRendererComponent(const glm::vec4& color)
		: m_Color(color)
	{
	}

	SpriteRendererComponent::SpriteRendererComponent(const SpriteRendererComponent& other)
		: m_Color(other.m_Color)
	{
	}

	SpriteRendererComponent& SpriteRendererComponent::operator=(const SpriteRendererComponent& other)
	{
		m_Color = other.m_Color;
		return *this;
	}

	SpriteRendererComponent::SpriteRendererComponent(SpriteRendererComponent&& other) noexcept
		: m_Color(std::move(other.m_Color))
	{
	}

	SpriteRendererComponent& SpriteRendererComponent::operator=(SpriteRendererComponent&& other) noexcept
	{
		m_Color = std::move(other.m_Color);
		return *this;
	}

	bool SpriteRendererComponent::operator==(const SpriteRendererComponent& other) const
	{
		return (m_Color == other.m_Color);
	}

	bool SpriteRendererComponent::operator!=(const SpriteRendererComponent& other) const
	{
		return !(*this == other);
	}

	SpriteRendererComponent& SpriteRendererComponent::SetColor(const glm::vec4& color)
	{
		m_Color = color;
		return *this;
	}

	const glm::vec4& SpriteRendererComponent::GetColor() const
	{
		return m_Color;
	}

	HP_REFLECT_TYPE
	{
		rttr::registration::class_<SpriteRendererComponent>("Sprite Renderer Component")
			(
				rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<SpriteRendererComponent>),
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.constructor<>()
			.constructor<const glm::vec4&>()
			.property("Color", &SpriteRendererComponent::GetColor, &SpriteRendererComponent::SetColor)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
				rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
			);
	}
}
