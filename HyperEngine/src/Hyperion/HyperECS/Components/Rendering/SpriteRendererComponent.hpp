#pragma once

#include <glm/glm.hpp>

namespace Hyperion
{
	class SpriteRendererComponent
	{
	private:
		glm::vec4 m_Color;

	public:
		SpriteRendererComponent() = default;
		~SpriteRendererComponent() = default;

		SpriteRendererComponent(const glm::vec4& color);

		SpriteRendererComponent(const SpriteRendererComponent& other);
		SpriteRendererComponent& operator=(const SpriteRendererComponent& other);

		SpriteRendererComponent(SpriteRendererComponent&& other) noexcept;
		SpriteRendererComponent& operator=(SpriteRendererComponent&& other) noexcept;

		bool operator==(const SpriteRendererComponent& other) const;
		bool operator!=(const SpriteRendererComponent& other) const;

		SpriteRendererComponent& SetColor(const glm::vec4& color);
		const glm::vec4& GetColor() const;
	};
}
