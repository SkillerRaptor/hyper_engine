#pragma once

#include <glm/glm.hpp>

#include "HyperRendering/TextureManager.hpp"

namespace HyperECS
{
	class SpriteRendererComponent
	{
	private:
		glm::vec4 m_Color;
		HyperRendering::TextureHandle m_Texture;

	public:
		SpriteRendererComponent() = default;
		~SpriteRendererComponent() = default;

		SpriteRendererComponent(const glm::vec4& color, HyperRendering::TextureHandle texture);

		SpriteRendererComponent(const SpriteRendererComponent& other);
		SpriteRendererComponent& operator=(const SpriteRendererComponent& other);

		SpriteRendererComponent(SpriteRendererComponent&& other) noexcept;
		SpriteRendererComponent& operator=(SpriteRendererComponent&& other) noexcept;

		bool operator==(const SpriteRendererComponent& other) const;
		bool operator!=(const SpriteRendererComponent& other) const;

		inline SpriteRendererComponent& SetColor(const glm::vec4& color)
		{
			m_Color = color;
			return *this;
		}

		inline const glm::vec4& GetColor() const
		{
			return m_Color;
		}

		inline SpriteRendererComponent& SetTexture(HyperRendering::TextureHandle texture)
		{
			m_Texture = texture;
			return *this;
		}

		inline HyperRendering::TextureHandle GetTexture() const
		{
			return m_Texture;
		}
	};
}
