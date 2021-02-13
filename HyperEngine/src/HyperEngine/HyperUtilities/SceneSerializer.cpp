#include "HyperUtilities/SceneSerializer.hpp"

#include <fstream>

#include "HyperECS/HyperComponents/Components.hpp"
#include "HyperUtilities/Reflection.hpp"

namespace HyperUtilities
{
	SceneSerializer::SceneSerializer(const HyperCore::Ref<HyperRendering::Scene>& scene)
		: m_Scene{ scene }
	{
	}

	bool SceneSerializer::Serialize(const std::string& filePath)
	{
		HyperECS::Registry& registry = m_Scene->GetRegistry();

		nlohmann::ordered_json configuration;
		std::vector<nlohmann::ordered_json> entities;
		for (HyperECS::Entity entity : registry.GetEntities())
		{
			if (entity == HyperECS::Null)
				continue;

			nlohmann::ordered_json entityJson;

			entityJson["Entity"] = registry.GetComponent<HyperECS::TagComponent>(entity).GetTag();

			SerializeImplement<HyperECS::TransformComponent>(entity, entityJson);
			SerializeImplement<HyperECS::SpriteRendererComponent>(entity, entityJson);
			SerializeImplement<HyperECS::CameraComponent>(entity, entityJson);

			entities.push_back(entityJson);
		}

		configuration["Scene"] = m_Scene->GetName();
		configuration["Entities"] = entities;

		std::ofstream file{ filePath };
		file << configuration.dump(4);
		return false;
	}

	bool SceneSerializer::SerializeRuntime(const std::string& filePath)
	{
		return false;
	}

	bool SceneSerializer::Deserialize(const std::string& filePath)
	{
		std::ifstream file{ filePath };

		nlohmann::json configuration = nlohmann::json::parse(file);
		m_Scene->SetName(configuration.contains("Scene") ? configuration["Scene"] : "Undefined!");
		if (configuration.contains("Entities"))
		{
			nlohmann::json entityJson = configuration["Entities"];
			for (nlohmann::json entityData : entityJson)
			{
				if (!entityData.contains("Entity"))
					continue;

				HyperECS::Entity entity = m_Scene->CreateEntity(entityData["Entity"]);

				DeserializeImplement<HyperECS::TransformComponent>(entity, entityData);
				DeserializeImplement<HyperECS::SpriteRendererComponent>(entity, entityData);
				DeserializeImplement<HyperECS::CameraComponent>(entity, entityData);
			}
		}

		return false;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		return false;
	}
}
