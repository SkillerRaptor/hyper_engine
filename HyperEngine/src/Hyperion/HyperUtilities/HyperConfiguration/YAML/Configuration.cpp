#include "Configuration.hpp"

#include <fstream>

namespace Hyperion { namespace YAML
{
	Configuration::Configuration(const std::string& filePath)
		: m_Data({}), m_FilePath(filePath)
	{
	}

	void Configuration::Parse()
	{

	}

	void Configuration::Write()
	{
		std::ofstream config(m_FilePath);

		size_t index = 0;

		for (ConfigurationValue& configurationValue : m_Data)
		{
			config << configurationValue.Key << ": ";
			if (auto value = std::get_if<std::string>(&configurationValue.Value))
				config << *value;
			else if (auto value = std::get_if<int>(&configurationValue.Value))
				config << *value;
			else if (auto value = std::get_if<uint8_t>(&configurationValue.Value))
				config << *value;
			else if (auto value = std::get_if<uint16_t>(&configurationValue.Value))
				config << *value;
			else if (auto value = std::get_if<uint32_t>(&configurationValue.Value))
				config << *value;
			else if (auto value = std::get_if<uint64_t>(&configurationValue.Value))
				config << *value;
			else if (auto value = std::get_if<float>(&configurationValue.Value))
				config << *value;
			else if (auto value = std::get_if<double>(&configurationValue.Value))
				config << *value;
			else if (auto value = std::get_if<Node>(&configurationValue.Value))
				value->Write(config, false, 0);
			else if (auto value = std::get_if<std::vector<Node>>(&configurationValue.Value))
				for (auto& node : *value)
					node.Write(config, true, 2);
			else if (auto value = std::get_if<Vec2>(&configurationValue.Value))
				config << "[" << value->x << ", " << value->y << "]";
			else if (auto value = std::get_if<Vec3>(&configurationValue.Value))
				config << "[" << value->x << ", " << value->y << ", " << value->z << "]";
			else if (auto value = std::get_if<Vec4>(&configurationValue.Value))
				config << "[" << value->x << ", " << value->y << ", " << value->z << ", " << value->w << "]";
			index++;
			if (index < m_Data.size()) config << "\n";
		}
		config.close();
	}
}}
