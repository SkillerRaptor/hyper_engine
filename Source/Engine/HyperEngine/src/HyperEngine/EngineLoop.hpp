/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperPlatform
{
	class IWindow;
}

namespace HyperEngine
{
	class IApplication;
	
	class CEngineLoop
	{
	public:
		void initialize(IApplication* application);
		void shutdown();
		
		void run();
	
	private:
		bool m_running{ false };
		IApplication* m_application{ nullptr };
		HyperPlatform::IWindow* m_window{ nullptr };
	};
}
