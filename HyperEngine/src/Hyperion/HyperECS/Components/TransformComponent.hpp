#pragma once

#include <glm/glm.hpp>

namespace Hyperion
{
	class TransformComponent
	{
	private:
		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_Scale;

	public:
		TransformComponent() = default;
		~TransformComponent() = default;

		TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

		TransformComponent(const TransformComponent& other);
		TransformComponent& operator=(const TransformComponent& other);

		TransformComponent(TransformComponent&& other) noexcept;
		TransformComponent& operator=(TransformComponent&& other) noexcept;

		bool operator==(const TransformComponent& other) const;
		bool operator!=(const TransformComponent& other) const;

		TransformComponent& SetPosition(const glm::vec3& position);
		const glm::vec3& GetPosition() const;

		TransformComponent& SetRotation(const glm::vec3& rotation);
		const glm::vec3& GetRotation() const;

		TransformComponent& SetScale(const glm::vec3& scale);
		const glm::vec3& GetScale() const;
	};
}
