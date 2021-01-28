#pragma once

#include "Buffers/OpenGL46FrameBuffer.hpp"
#include "HyperCore/Core.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperRendering/SceneRecorder.hpp"

namespace Hyperion
{
	class OpenGL46SceneRecorder : public SceneRecorder
	{
	private:
		Scope<OpenGL46FrameBuffer> m_GameFrameBuffer;
		Scope<OpenGL46FrameBuffer> m_EditorFrameBuffer;

	public:
		OpenGL46SceneRecorder(Ref<TextureManager> textureManager, GLFWwindow* window);

		virtual void InitRecorder() override;
		virtual void RebuildRecoder() override;

		virtual void StartGameRecording() override;
		virtual void EndGameRecording() override;
		virtual void RenderGameImage() override;

		virtual void StartEditorRecording() override;
		virtual void EndEditorRecording() override;
		virtual void RenderEditorImage() override;

		virtual TextureHandle GetGameColorAttachment() override;
		virtual TextureHandle GetGameDepthAttachment() override;
		virtual TextureHandle GetEditorColorAttachment() override;
		virtual TextureHandle GetEditorDepthAttachment() override;

		virtual void SetGameViewportSize(const glm::vec2& gameViewportSize) override;
		virtual glm::vec2 GetGameViewportSize() override;
		virtual void SetEditorViewportSize(const glm::vec2& editorViewportSize) override;
		virtual glm::vec2 GetEditorViewportSize() override;
	};
}
