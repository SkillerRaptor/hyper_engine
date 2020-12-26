#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <variant>
#include <vector>

#include "Node.hpp"
#include "HyperCore/Core.hpp"

namespace Hyperion { namespace YAML
{
	class Configuration
	{
	private:
		using ConfigurationType = std::variant<std::string, int, uint8_t, uint16_t, uint32_t, uint64_t, float, double, Node, std::vector<Node>, glm::vec2, glm::vec3, glm::vec4>;

		struct ConfigurationValue
		{
			std::string Key;
			ConfigurationType Value;

			ConfigurationValue(const std::string& key)
				: Key(key), Value() {}
		};

		std::vector<ConfigurationValue> m_Data;
		std::string m_FilePath;

	public:
		Configuration(const std::string& filePath);

		void Parse();
		void Write();

		ConfigurationType& operator[](const std::string& key)
		{
			for (auto& value : m_Data)
				if (value.Key == key)
					return value.Value;
			ConfigurationValue& value = m_Data.emplace_back(key);
			return value.Value;
		}
	};
}}
