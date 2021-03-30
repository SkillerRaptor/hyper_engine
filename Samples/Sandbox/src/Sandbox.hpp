#pragma once

#include <HyperEngine.hpp>

using namespace HyperEngine;

class Sandbox : public Application
{
public:
	virtual void OnInitialize() override;
	virtual void OnTerminate() override;
	
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnLateUpdate(float deltaTime) override;
	virtual void OnRender() override;
};