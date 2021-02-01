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
		nlohmann::ordered_json configuration;

		std::vector<nlohmann::ordered_json> entities;
		m_Scene->Each([&](HyperEntity entity)
			{
				if (!entity)
					return;

				nlohmann::ordered_json entityJson;

				entityJson["Entity"] = entity.GetComponent<TagComponent>().GetTag();

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
			for (nlohmann::json entity : entityJson)
			{
				if(!entity.contains("Entity"))
					continue;

				HyperEntity hyperEntity = m_Scene->CreateEntity(entity["Entity"]);

				DeserializeImplement<TransformComponent>(hyperEntity, entity);
				DeserializeImplement<SpriteRendererComponent>(hyperEntity, entity);
				DeserializeImplement<CameraComponent>(hyperEntity, entity);
			}
		}

		return false;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		return false;
	}
}
