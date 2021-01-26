#pragma once

#include "TextureManager.hpp"
#include "HyperCore/Core.hpp"

struct GLFWwindow;

namespace Hyperion
{
	class SceneRecorder
	{
	protected:
		Ref<TextureManager> m_TextureManager;
		GLFWwindow* m_Window;

	public:
		SceneRecorder(Ref<TextureManager> textureManager, GLFWwindow* window)
			: m_TextureManager(textureManager), m_Window(window) {}

		virtual void InitRecording() = 0;
		virtual void StartRecording() = 0;
		virtual void EndRecording() = 0;

		virtual void RenderImage() = 0;
	};
}
