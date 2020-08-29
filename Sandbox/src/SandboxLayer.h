#pragma once

#include <Hyperion.h>

using namespace Hyperion;

class SandboxLayer : public Layer
{
private:
public:
	SandboxLayer();
	virtual ~SandboxLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnEvent(Event& event) override;
	virtual void OnTick(int currentTick) override;
	virtual void OnUpdate(Timestep timeStep) override;
	virtual void OnRender() override;
};