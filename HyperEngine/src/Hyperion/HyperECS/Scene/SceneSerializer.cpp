#include "SceneSerializer.hpp"

#include <nlohmann/json.hpp>

#include <fstream>

#include "HyperECS/Components.hpp"
#include "HyperECS/HyperEntity.hpp"

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

				entityJson["Entity"] = entity.GetComponent<TagComponent>().Tag;

				if (entity.HasComponent<TransformComponent>())
				{
					nlohmann::ordered_json componentData;
					auto& component = entity.GetComponent<TransformComponent>();

					std::vector<float> position = { component.Position.x, component.Position.y, component.Position.z };
					componentData["Position"] = position;

					std::vector<float> rotation = { component.Rotation.x, component.Rotation.y, component.Rotation.z };
					componentData["Rotation"] = rotation;

					std::vector<float> scale = { component.Scale.x, component.Scale.y, component.Scale.z };
					componentData["Scale"] = scale;

					entityJson["Transform Component"] = componentData;
				}

				if (entity.HasComponent<SpriteRendererComponent>())
				{
					nlohmann::ordered_json componentData;
					auto& component = entity.GetComponent<SpriteRendererComponent>();

					std::vector<float> color = { component.Color.x, component.Color.y, component.Color.z, component.Color.w };
					componentData["Color"] = color;

					entityJson["Sprite Renderer Component"] = componentData;
				}

				if (entity.HasComponent<CameraComponent>())
				{
					nlohmann::ordered_json componentData;
					auto& component = entity.GetComponent<CameraComponent>();

					std::vector<float> backgroundColor = { component.BackgroundColor.x, component.BackgroundColor.y, component.BackgroundColor.z, component.BackgroundColor.w };
					componentData["Background Color"] = backgroundColor;

					std::string currentProjection;
					switch (component.Projection)
					{
					case CameraComponent::ProjectionType::ORTHOGRAPHIC:
						currentProjection = "Orthographic";
						break;
					case CameraComponent::ProjectionType::PERSPECTIVE:
						currentProjection = "Perspective";
						break;
					default:
						break;
					}
					componentData["Projection"] = currentProjection;

					componentData["FOV"] = component.FOV;

					std::vector<float> clippingPlanes = { component.ClippingPlanes.x, component.ClippingPlanes.y };
					componentData["Clipping Planes"] = clippingPlanes;
					
					std::vector<float> viewportRect = { component.ViewportRect.x, component.ViewportRect.y };
					componentData["Viewport Rect"] = viewportRect;

					componentData["Primary"] = component.Primary;

					entityJson["Camera Component"] = componentData;
				}

				if (entity.HasComponent<CameraControllerComponent>())
				{
					nlohmann::ordered_json componentData;
					auto& component = entity.GetComponent<CameraControllerComponent>();

					componentData["Move Speed"] = component.MoveSpeed;
					componentData["Zoom Speed"] = component.ZoomSpeed;

					entityJson["Camera Controller Component"] = componentData;
				}

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

				if (entity.contains("Transform Component"))
				{
					nlohmann::json componentData = entity["Transform Component"];
					TransformComponent& component = hyperEntity.GetComponent<TransformComponent>();

					std::vector<float> position = componentData["Position"];
					component.Position = glm::vec3{ position[0], position[1], position[2] };

					std::vector<float> rotation = componentData["Rotation"];
					component.Rotation = glm::vec3{ rotation[0], rotation[1], rotation[2] };

					std::vector<float> scale = componentData["Scale"];
					component.Scale = glm::vec3{ scale[0], scale[1], scale[2] };
				}

				if (entity.contains("Sprite Renderer Component"))
				{
					nlohmann::json componentData = entity["Sprite Renderer Component"];
					SpriteRendererComponent& component = hyperEntity.AddComponent<SpriteRendererComponent>();

					std::vector<float> color = componentData["Color"];
					component.Color = glm::vec4{ color[0], color[1], color[2], color[3] };
				}

				if (entity.contains("Camera Component"))
				{
					nlohmann::json componentData = entity["Camera Component"];
					CameraComponent& component = hyperEntity.AddComponent<CameraComponent>();

					std::vector<float> backgroundColor = componentData["Background Color"];
					component.BackgroundColor = glm::vec4{ backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3] };

					std::string currentProjection = componentData["Projection"];
					if (currentProjection == "Orthographic")
						component.Projection = CameraComponent::ProjectionType::ORTHOGRAPHIC;
					else if (currentProjection == "Perspective")
						component.Projection = CameraComponent::ProjectionType::PERSPECTIVE;

					component.FOV = componentData["FOV"];

					std::vector<float> clippingPlanes = componentData["Clipping Planes"];
					component.ClippingPlanes = glm::vec2{ clippingPlanes[0], clippingPlanes[1] };

					std::vector<float> viewportRect = componentData["Viewport Rect"];
					component.ViewportRect = glm::vec2{ viewportRect[0], viewportRect[1] };

					component.Primary = componentData["Primary"];
				}

				if (entity.contains("Camera Controller Component"))
				{
					nlohmann::json componentData = entity["Camera Controller Component"];
					CameraControllerComponent& component = hyperEntity.AddComponent<CameraControllerComponent>();

					component.MoveSpeed = componentData["Move Speed"];
					component.ZoomSpeed = componentData["Zoom Speed"];
				}
			}
		}

		return false;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		return false;
	}
}
