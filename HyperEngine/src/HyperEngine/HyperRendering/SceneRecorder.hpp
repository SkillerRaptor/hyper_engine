#pragma once

#include "HyperCore/Core.hpp"
#include "HyperRendering/TextureManager.hpp"

struct GLFWwindow;

namespace HyperRendering
{
	class SceneRecorder
	{
	protected:
		HyperCore::Ref<TextureManager> m_TextureManager;
		GLFWwindow* m_Window;

		glm::vec2 m_GameViewportSize{ 0.0f };
		glm::vec2 m_EditorViewportSize{ 0.0f };

		bool m_GameFocused{ false };
		bool m_EditorFocused{ false };

		bool m_RebuildGameImage{ false };
		bool m_RebuildEditorImage{ false };

	public:
		SceneRecorder(HyperCore::Ref<TextureManager> textureManager, GLFWwindow* window)
			: m_TextureManager{ textureManager }, m_Window{ window }
		{
		}

		virtual void InitRecorder() = 0;
		virtual void RebuildRecoder() = 0;

		virtual void StartGameRecording() = 0;
		virtual void EndGameRecording() = 0;
		virtual void RenderGameImage() = 0;

		virtual void StartEditorRecording() = 0;
		virtual void EndEditorRecording() = 0;
		virtual void RenderEditorImage() = 0;

		virtual inline TextureHandle GetGameColorAttachment() = 0;
		virtual inline TextureHandle GetGameDepthAttachment() = 0;
		virtual inline TextureHandle GetEditorColorAttachment() = 0;
		virtual inline TextureHandle GetEditorDepthAttachment() = 0;

		virtual inline void SetGameViewportSize(const glm::vec2& gameViewportSize) = 0;
		virtual inline const glm::vec2& GetGameViewportSize() const = 0;
		virtual inline void SetEditorViewportSize(const glm::vec2& editorViewportSize) = 0;
		virtual inline const glm::vec2& GetEditorViewportSize() const = 0;

		inline void SetGameFocused(bool gameFocused)
		{
			m_GameFocused = gameFocused;
		}

		inline bool IsGameFocused() const
		{
			return m_GameFocused;
		}

		inline void SetEditorFocused(bool editorFocused)
		{
			m_EditorFocused = editorFocused;
		}

		inline bool IsEditorFocused() const
		{
			return m_EditorFocused;
		}
	};
}
