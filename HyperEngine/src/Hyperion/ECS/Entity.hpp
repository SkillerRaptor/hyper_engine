#pragma once

#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Component.hpp"

namespace Hyperion
{
	class Entity
	{
	private:
		unsigned int m_Id;
		std::unordered_map<std::string, Component*> m_Components;

	public:
		Entity(unsigned int id);
		~Entity();

		template<class T>
		bool HasComponent()
		{
			return m_Components.find(std::type_index(typeid(T)).name()) != m_Components.end();
		}

		template<class T>
		T* GetComponent()
		{
			return HasComponent<T>() ? m_Components[std::type_index(typeid(T)).name()] : nullptr;
		}
	};
}