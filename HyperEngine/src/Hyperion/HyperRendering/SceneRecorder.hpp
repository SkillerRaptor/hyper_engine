#pragma once

#include "HyperCore/Core.hpp"
#include "HyperRendering/RenderContext.hpp"

namespace Hyperion
{
	class SceneRecorder
	{
	protected:
		Ref<RenderContext> m_RenderContext;

	public:
		virtual ~SceneRecorder() = default;

		virtual void InitRecording() = 0;
		virtual void StartRecording() = 0;
		virtual void EndRecording() = 0;

		virtual void RenderImage() {};

		static Ref<SceneRecorder> Construct(Ref<RenderContext> renderContext);
	
	protected:
		SceneRecorder(Ref<RenderContext> renderContext)
			: m_RenderContext(renderContext) {}
	};
}
