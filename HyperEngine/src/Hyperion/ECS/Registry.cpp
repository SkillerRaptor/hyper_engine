#include "Registry.hpp"

#include "Components.hpp"

namespace Hyperion
{
	Registry::Registry()
	{
	}

	Registry::~Registry()
	{
	}

	uint32_t Registry::ConstructEntity(const std::string& name)
	{
		uint32_t entityId = Hasher::PrimeHasher(name);
		m_Entities.emplace(entityId, std::unordered_map<uint32_t, size_t>());

		Mat4 transform = Mat4();
		transform += Matrix::Translate(Mat4(1.0f), Vec3(0.0f));
		//transform = Matrix::Rotate(Mat4(1.0f), Vec3(0.0f));
		transform += Matrix::Scale(Mat4(1.0f), Vec3(10.0f));

		AddComponent<TransformComponent>(entityId, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(10.0f, 10.0f, 10.0f));
		AddComponent<TagComponent>(entityId, name.empty() ? "Entity" : name);

		return entityId;
	}

	void Registry::DeleteEntity(uint32_t entity)
	{
		if (m_Entities.find(entity) == m_Entities.end())
			return;
		m_Entities.erase(entity);
	}
}
