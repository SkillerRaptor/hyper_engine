#pragma once

#include <nlohmann/json.hpp>

#include "HyperCore/Core.hpp"
#include "HyperECS/HyperEntity.hpp"
#include "HyperECS/Scene/Scene.hpp"

namespace Hyperion
{
	class SceneSerializer
	{
	private:
		Ref<Scene> m_Scene;

	public:
		SceneSerializer(const Ref<Scene>& scene);

		bool Serialize(const std::string& filePath);
		bool SerializeRuntime(const std::string& filePath);

		bool Deserialize(const std::string& filePath);
		bool DeserializeRuntime(const std::string& filePath);

	private:
		template<typename T>
		void SerializeImplement(HyperEntity entity, nlohmann::ordered_json& json)
		{
			if (entity.HasComponent<T>())
			{
				T& component = entity.GetComponent<T>();

				rttr::instance instance = component;
				rttr::type type = instance.get_type();

				ReflectionMeta componentMeta(type);
				if ((componentMeta.Flags & MetaInfo::SERIALIZABLE) == 0) return;

				nlohmann::ordered_json componentData;

				for (auto& property : type.get_properties())
				{
					ReflectionMeta propertyMeta(property);
					if ((propertyMeta.Flags & MetaInfo::SERIALIZABLE) == 0) continue;

					const char* propertyName = property.get_name().cbegin();
					rttr::variant propertyValue = property.get_value(instance);
					rttr::type propertyType = property.get_type();

					if (propertyType.is_arithmetic())
					{
						if (propertyValue.is_type<bool>())
							componentData[propertyName] = propertyValue.to_bool();
						else if (propertyValue.is_type<float>())
							componentData[propertyName] = propertyValue.to_float();
						continue;
					}

					if (propertyType.is_enumeration())
					{
						componentData[propertyName] = propertyValue.to_string();
						continue;
					}

					std::string typeName = propertyType.get_name().cbegin();
					if (typeName == "Vector2")
					{
						std::array<float, 2> array;
						size_t i = 0;
						for (auto& propertyProperty : propertyType.get_properties())
						{
							rttr::variant propertyPropertyValue = propertyProperty.get_value(propertyValue);
							array[i] = propertyPropertyValue.to_float();
							i++;
						}
						componentData[propertyName] = array;
					}
					else if (typeName == "Vector3")
					{
						std::array<float, 3> array;
						size_t i = 0;
						for (auto& propertyProperty : propertyType.get_properties())
						{
							rttr::variant propertyPropertyValue = propertyProperty.get_value(propertyValue);
							array[i] = propertyPropertyValue.to_float();
							i++;
						}
						componentData[propertyName] = array;
					}
					else if (typeName == "Vector4")
					{
						std::array<float, 4> array;
						size_t i = 0;
						for (auto& propertyProperty : propertyType.get_properties())
						{
							rttr::variant propertyPropertyValue = propertyProperty.get_value(propertyValue);
							array[i] = propertyPropertyValue.to_float();
							i++;
						}
						componentData[propertyName] = array;
					}
				}

				json[type.get_name().cbegin()] = componentData;
			}
		}

		template<typename T>
		void DeserializeImplement(HyperEntity entity, const nlohmann::ordered_json& json)
		{
			rttr::type type = rttr::type::get<T>();
			const char* key = type.get_name().cbegin();
			if (json.contains(key))
			{
				if (!entity.HasComponent<T>())
					entity.AddComponent<T>();

				T& component = entity.GetComponent<T>();
				rttr::instance componentInstance = component;
				nlohmann::json componentData = json[key];

				for (auto& property : type.get_properties())
				{
					ReflectionMeta propertyMeta(property);
					rttr::type propertyType = property.get_type();
					std::string typeName = propertyType.get_name().cbegin();
					const char* propertyName = property.get_name().cbegin();

					if (propertyType.is_enumeration())
					{
						rttr::enumeration e = propertyType.get_enumeration();
						property.set_value(component, e.name_to_value(static_cast<std::string>(componentData[propertyName])));
					}
					else if (typeName == "bool")
					{
						property.set_value(component, static_cast<bool>(componentData[propertyName]));
					}
					else if (typeName == "float")
					{
						property.set_value(component, static_cast<float>(componentData[propertyName]));
					}
					else if (typeName == "Vector2")
					{
						std::array<float, 2> array = componentData[propertyName];
						property.set_value(component, glm::vec2 { array[0], array[1] });
					}
					else if (typeName == "Vector3")
					{
						std::array<float, 3> array = componentData[propertyName];
						property.set_value(component, glm::vec3 { array[0], array[1], array[2] });
					}
					else if (typeName == "Vector4")
					{
						std::array<float, 4> array = componentData[propertyName];
						property.set_value(component, glm::vec4 { array[0], array[1], array[2], array[3] });
					}
				}
			}
		}
	};
}
