#pragma once

#include "HyperCore/Core.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperRendering/SceneRecorder.hpp"

#include "Platform/Rendering/OpenGL33/Buffers/OpenGL33FrameBuffer.hpp"

namespace Hyperion
{
	class OpenGL33SceneRecorder : public SceneRecorder
	{
	private:
		Scope<OpenGL33FrameBuffer> m_GameFrameBuffer;
		Scope<OpenGL33FrameBuffer> m_EditorFrameBuffer;

	public:
		OpenGL33SceneRecorder(Ref<TextureManager> textureManager, GLFWwindow* window);

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
