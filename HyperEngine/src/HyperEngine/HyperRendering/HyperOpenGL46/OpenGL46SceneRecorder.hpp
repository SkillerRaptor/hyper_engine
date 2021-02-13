#pragma once

#include "HyperCore/Core.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperRendering/SceneRecorder.hpp"
#include "HyperRendering/HyperOpenGL46/HyperBuffers/OpenGL46FrameBuffer.hpp"

namespace HyperRendering
{
	class OpenGL46SceneRecorder : public SceneRecorder
	{
	private:
		HyperCore::Scope<OpenGL46FrameBuffer> m_GameFrameBuffer;
		HyperCore::Scope<OpenGL46FrameBuffer> m_EditorFrameBuffer;

	public:
		OpenGL46SceneRecorder(HyperCore::Ref<TextureManager> textureManager, GLFWwindow* window);

		virtual void InitRecorder() override;
		virtual void RebuildRecoder() override;

		virtual void StartGameRecording() override;
		virtual void EndGameRecording() override;
		virtual void RenderGameImage() override;

		virtual void StartEditorRecording() override;
		virtual void EndEditorRecording() override;
		virtual void RenderEditorImage() override;

		virtual inline TextureHandle GetGameColorAttachment() override
		{
			return m_GameFrameBuffer ? m_GameFrameBuffer->GetColorAttachment() : TextureHandle{ 0 };
		}

		virtual inline TextureHandle GetGameDepthAttachment() override
		{
			return m_GameFrameBuffer ? m_GameFrameBuffer->GetDepthAttachment() : TextureHandle{ 0 };
		}

		virtual inline TextureHandle GetEditorColorAttachment() override
		{
			return m_EditorFrameBuffer ? m_EditorFrameBuffer->GetColorAttachment() : TextureHandle{ 0 };
		}

		virtual inline TextureHandle GetEditorDepthAttachment() override
		{
			return m_EditorFrameBuffer ? m_EditorFrameBuffer->GetDepthAttachment() : TextureHandle{ 0 };
		}

		virtual inline void SetGameViewportSize(const glm::vec2& gameViewportSize) override
		{
			if (!(gameViewportSize.x > 0 && gameViewportSize.x < 8096 && gameViewportSize.y > 0 && gameViewportSize.y < 8096))
				return;
			m_GameViewportSize = gameViewportSize;
			if (m_GameFrameBuffer->GetWidth() != m_GameViewportSize.x || m_GameFrameBuffer->GetHeight() != m_GameViewportSize.y)
				m_RebuildGameImage = true;
		}

		virtual inline const glm::vec2& GetGameViewportSize() const override
		{
			return m_GameViewportSize;
		}

		virtual inline void SetEditorViewportSize(const glm::vec2& editorViewportSize) override
		{
			if (!(editorViewportSize.x > 0 && editorViewportSize.x < 8096 && editorViewportSize.y > 0 && editorViewportSize.y < 8096))
				return;
			m_EditorViewportSize = editorViewportSize;
			if (m_EditorFrameBuffer->GetWidth() != m_EditorViewportSize.x || m_EditorFrameBuffer->GetHeight() != m_EditorViewportSize.y)
				m_RebuildEditorImage = true;
		}

		virtual inline const glm::vec2& GetEditorViewportSize() const override
		{
			return m_EditorViewportSize;
		}
	};
}
