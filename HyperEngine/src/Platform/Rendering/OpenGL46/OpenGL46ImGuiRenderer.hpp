#pragma once

#include "HyperCore/Core.hpp"
#include "HyperRendering/RenderContext.hpp"

#include "Platform/Rendering/ImGui/ImGuiRenderer.hpp"
#include "Platform/Rendering/OpenGL46/Buffers/OpenGL46FrameBuffer.hpp"

namespace Hyperion
{
	class OpenGL46ImGuiRenderer : public ImGuiRenderer
	{
	private:
		uint32_t m_BufferTexture;
		uint32_t m_BufferTextureId;
		Scope<OpenGL46FrameBuffer> m_FrameBuffer;

	public:
		OpenGL46ImGuiRenderer(Ref<RenderContext> renderContext);

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
