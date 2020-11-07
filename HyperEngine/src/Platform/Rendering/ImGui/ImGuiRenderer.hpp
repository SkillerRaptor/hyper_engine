#pragma once

#include "HyperCore/Core.hpp"
#include "HyperRendering/RenderContext.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	class ImGuiRenderer
	{
	protected:
		Ref<RenderContext> m_RenderContext;

	public:
		ImGuiRenderer(Ref<RenderContext> renderContext)
			: m_RenderContext(renderContext) {}

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		virtual void OnUpdate(Timestep timeStep) = 0;
		virtual void OnRender() = 0;

		virtual void RenderImage() = 0;

		virtual void InitCapture() = 0;
		virtual void StartCapture() = 0;
		virtual void EndCapture() = 0;

		static Ref<ImGuiRenderer> Construct(Ref<RenderContext> renderContext);
	};
}
