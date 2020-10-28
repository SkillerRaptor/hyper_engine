#pragma once

#include <string>
#include <fstream>
#include <variant>

#include "Core/Core.hpp"
#include "HyperMath/HyperMath.hpp"

namespace Hyperion
{
	namespace YAML
	{
		class Node
		{
		private:
			using NodeType = std::variant<std::string, int, uint8_t, uint16_t, uint32_t, uint64_t, float, double, Node*, std::vector<Node>, Vec2, Vec3, Vec4>;

			struct NodeValue
			{
				std::string Key;
				NodeType Value;

				NodeValue(const std::string& key)
					: Key(key), Value() {}
			};

			std::vector<NodeValue> m_Data;

		public:
			Node();

			void Parse(std::istream& is);
			void Write(std::ostream& os, bool line, uint8_t spaces);

			NodeType& operator[](const std::string& key)
			{
				for (auto& value : m_Data)
					if (value.Key == key)
						return value.Value;
				NodeValue& value = m_Data.emplace_back(key);
				return value.Value;
			}
		};
	}
}
