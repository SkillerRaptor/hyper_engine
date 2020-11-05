#pragma once

#include "Core/Core.hpp"
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

		bool Deserialize(const std::string& filePath);
	};
}
