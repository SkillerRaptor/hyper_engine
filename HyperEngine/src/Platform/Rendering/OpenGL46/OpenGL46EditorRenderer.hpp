#pragma once

#include "HyperCore/Core.hpp"
#include "HyperRendering/RenderContext.hpp"

#include "Platform/Rendering/ImGui/EditorRenderer.hpp"
#include "Platform/Rendering/OpenGL46/Buffers/OpenGL46FrameBuffer.hpp"

namespace Hyperion
{
	class OpenGL46EditorRenderer : public EditorRenderer
	{
	private:
		TextureHandle m_BufferTexture;
		uint32_t m_BufferTextureId;
		Scope<OpenGL46FrameBuffer> m_FrameBuffer;

	public:
		OpenGL46EditorRenderer(Ref<RenderContext> renderContext);

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;

		virtual void RenderImage() override;

		virtual void InitCapture() override;
		virtual void StartCapture() override;
		virtual void EndCapture() override;
	};
}
