#include "SpriteRendererComponent.hpp"

#include "HyperUtilities/Reflection.hpp"

namespace Hyperion
{
	SpriteRendererComponent::SpriteRendererComponent(const glm::vec4& color, TextureHandle texture)
		: m_Color(color), m_Texture(texture)
	{
	}

	SpriteRendererComponent::SpriteRendererComponent(const SpriteRendererComponent& other)
		: m_Color(other.m_Color), m_Texture(other.m_Texture)
	{
	}

	SpriteRendererComponent& SpriteRendererComponent::operator=(const SpriteRendererComponent& other)
	{
		m_Color = other.m_Color;
		m_Texture = other.m_Texture;
		return *this;
	}

	SpriteRendererComponent::SpriteRendererComponent(SpriteRendererComponent&& other) noexcept
		: m_Color(std::move(other.m_Color)), m_Texture(std::move(other.m_Texture))
	{
	}

	SpriteRendererComponent& SpriteRendererComponent::operator=(SpriteRendererComponent&& other) noexcept
	{
		m_Color = std::move(other.m_Color);
		m_Texture = std::move(other.m_Texture);
		return *this;
	}

	bool SpriteRendererComponent::operator==(const SpriteRendererComponent& other) const
	{
		return (m_Color == other.m_Color && m_Texture == other.m_Texture);
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

	SpriteRendererComponent& SpriteRendererComponent::SetTexture(TextureHandle texture)
	{
		m_Texture = texture;
		return *this;
	}

	TextureHandle SpriteRendererComponent::GetTexture() const
	{
		return m_Texture;
	}

	HP_REFLECT_TYPE
	{
		rttr::registration::class_<SpriteRendererComponent>("Sprite Renderer Component")
			(
				rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<SpriteRendererComponent>),
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.constructor<>()
			.constructor<const glm::vec4&, TextureHandle>()
			.property("Color", &SpriteRendererComponent::GetColor, &SpriteRendererComponent::SetColor)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
				rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
			)
			.property("Texture", &SpriteRendererComponent::GetTexture, &SpriteRendererComponent::SetTexture)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
			);
	}
}
