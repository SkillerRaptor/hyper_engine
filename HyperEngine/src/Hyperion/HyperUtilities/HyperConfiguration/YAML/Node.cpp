#include "Node.hpp"

#include <fstream>

namespace Hyperion { namespace YAML
{
	Node::Node()
		: m_Data({})
	{
	}

	void Node::Parse(std::istream& is)
	{
	}

	void Node::Write(std::ostream& os, bool line, uint8_t spaces)
	{
		os << "\n";
		size_t index = 0;

		for (NodeValue& nodeValue : m_Data)
		{
			for (uint8_t i = 0; i < spaces; i++)
				os << " ";
			os << ((index == 0 && line) ? "- " : "  ") << nodeValue.Key << ": ";
			if (auto value = std::get_if<std::string>(&nodeValue.Value))
				os << *value;
			else if (auto value = std::get_if<int>(&nodeValue.Value))
				os << *value;
			else if (auto value = std::get_if<uint8_t>(&nodeValue.Value))
				os << *value;
			else if (auto value = std::get_if<uint16_t>(&nodeValue.Value))
				os << *value;
			else if (auto value = std::get_if<uint32_t>(&nodeValue.Value))
				os << *value;
			else if (auto value = std::get_if<uint64_t>(&nodeValue.Value))
				os << *value;
			else if (auto value = std::get_if<float>(&nodeValue.Value))
				os << *value;
			else if (auto value = std::get_if<double>(&nodeValue.Value))
				os << *value;
			else if (auto value = std::get_if<Node*>(&nodeValue.Value))
				(*value)->Write(os, false, spaces + 2);
			else if (auto value = std::get_if<std::vector<Node>>(&nodeValue.Value))
				for (auto& node : *value)
					node.Write(os, true, spaces + 2);
			else if (auto value = std::get_if<glm::vec2>(&nodeValue.Value))
				os << "[" << value->x << ", " << value->y << "]";
			else if (auto value = std::get_if<glm::vec3>(&nodeValue.Value))
				os << "[" << value->x << ", " << value->y << ", " << value->z << "]";
			else if (auto value = std::get_if<glm::vec4>(&nodeValue.Value))
				os << "[" << value->x << ", " << value->y << ", " << value->z << ", " << value->w << "]";
			index++;
			if (index < m_Data.size()) os << "\n";
		}
	}
}}
