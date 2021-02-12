#pragma once

#include "HyperRendering/ImGuiRenderer.hpp"

namespace HyperRendering
{
	class OpenGL46ImGuiRenderer : public ImGuiRenderer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Start() override;
		virtual void End() override;
	};
}
