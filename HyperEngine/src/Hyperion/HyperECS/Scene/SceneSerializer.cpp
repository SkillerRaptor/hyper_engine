#include "SceneSerializer.hpp"

#include <fstream>

#include "HyperECS/Components/Components.hpp"
#include "HyperUtilities/Reflection.hpp"

namespace Hyperion
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	bool SceneSerializer::Serialize(const std::string& filePath)
	{
		Registry& registry = m_Scene->GetRegistry();

		nlohmann::ordered_json configuration;
		std::vector<nlohmann::ordered_json> entities;
		registry.Each([&](Entity entity)
			{
				if (entity == Null)
					return;

				nlohmann::ordered_json entityJson;

				entityJson["Entity"] = registry.GetComponent<TagComponent>(entity).GetTag();

				SerializeImplement<TransformComponent>(entity, entityJson);
				SerializeImplement<SpriteRendererComponent>(entity, entityJson);
				SerializeImplement<CameraComponent>(entity, entityJson);

				entities.push_back(entityJson);
			});

		configuration["Scene"] = m_Scene->GetName();
		configuration["Entities"] = entities;

		std::ofstream file(filePath);
		file << configuration.dump(4);
		return false;
	}

	bool SceneSerializer::SerializeRuntime(const std::string& filePath)
	{
		return false;
	}

	bool SceneSerializer::Deserialize(const std::string& filePath)
	{
		std::ifstream file(filePath);

		nlohmann::json configuration = nlohmann::json::parse(file);
		m_Scene->SetName(configuration.contains("Scene") ? configuration["Scene"] : "Undefined!");
		if (configuration.contains("Entities"))
		{
			nlohmann::json entityJson = configuration["Entities"];
			for (nlohmann::json entityData : entityJson)
			{
				if (!entityData.contains("Entity"))
					continue;

				Entity entity = m_Scene->CreateEntity(entityData["Entity"]);

				DeserializeImplement<TransformComponent>(entity, entityData);
				DeserializeImplement<SpriteRendererComponent>(entity, entityData);
				DeserializeImplement<CameraComponent>(entity, entityData);
			}
		}

		return false;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		return false;
	}
}
