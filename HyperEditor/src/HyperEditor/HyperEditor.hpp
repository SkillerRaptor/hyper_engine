#pragma once

#include <HyperSystem/Application.hpp>

using namespace HyperEngine;

namespace HyperEditor
{
	class HyperEditor : public Application
	{
		virtual void OnInitialize(ApplicationInfo& applicationInfo) override;
		virtual void OnTerminate() override;
	};
}
