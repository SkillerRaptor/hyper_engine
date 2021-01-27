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

		glm::vec2 m_GameViewportSize;
		glm::vec2 m_EditorViewportSize;

		bool m_GameFocused;
		bool m_EditorFocused;

		bool m_RebuildGameImage;
		bool m_RebuildEditorImage;

	public:
		SceneRecorder(Ref<TextureManager> textureManager, GLFWwindow* window)
			: m_TextureManager(textureManager), m_Window(window) {}

		virtual void InitRecorder() = 0;
		virtual void RebuildRecoder() = 0;

		virtual void StartGameRecording() = 0;
		virtual void EndGameRecording() = 0;
		virtual void RenderGameImage() = 0;

		virtual void StartEditorRecording() = 0;
		virtual void EndEditorRecording() = 0;
		virtual void RenderEditorImage() = 0;

		virtual TextureHandle GetGameColorAttachment() = 0;
		virtual TextureHandle GetGameDepthAttachment() = 0;
		virtual TextureHandle GetEditorColorAttachment() = 0;
		virtual TextureHandle GetEditorDepthAttachment() = 0;

		virtual void SetGameViewportSize(const glm::vec2& gameViewportSize) = 0;
		virtual glm::vec2 GetGameViewportSize() = 0;
		virtual void SetEditorViewportSize(const glm::vec2& editorViewportSize) = 0;
		virtual glm::vec2 GetEditorViewportSize() = 0;

		void SetGameFocused(bool gameFocused) { m_GameFocused = gameFocused; }
		bool IsGameFocused() const { return m_GameFocused; }
		void SetEditorFocused(bool editorFocused) { m_EditorFocused = editorFocused; }
		bool IsEditorFocused() const { return m_EditorFocused; }
	};
}
