#pragma once

#include "HyperCore/Core.hpp"
#include "HyperRendering/RenderContext.hpp"

namespace Hyperion
{
	class ImGuiRenderer
	{
	public:
		virtual ~ImGuiRenderer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		virtual void Start() = 0;
		virtual void End() = 0;

		static Ref<ImGuiRenderer> Construct(const Ref<RenderContext> renderContext);
	};
}
