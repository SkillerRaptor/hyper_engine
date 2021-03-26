#pragma once

#include <HyperGame/State.hpp>

using namespace HyperEngine;

namespace HyperEditor
{
	class ProjectSelectionState : public State
	{
	public:
		virtual void OnCreate() override;
		virtual void OnDestroy() override;
		
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnLateUpdate(float deltaTime) override;
		virtual void OnRender() override;
	};
}
