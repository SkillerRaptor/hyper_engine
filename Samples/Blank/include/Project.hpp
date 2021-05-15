#pragma once

#include <HyperEngine/HyperEngine.hpp>

class Project : public HyperEngine::Application
{
public:
	virtual void OnInitialize() override;
	virtual void OnTerminate() override;
	
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnLateUpdate(float deltaTime) override;
	virtual void OnRender() override;
};