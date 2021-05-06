#pragma once

#include <HyperEngine.hpp>

namespace HyperEditor
{
	class HyperEditor : public HyperEngine::Application
	{
		virtual void OnInitialize() override;
		virtual void OnTerminate() override;
		
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnLateUpdate(float deltaTime) override;
		virtual void OnRender() override;
	};
}
