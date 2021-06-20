/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/ILibraryManager.hpp>

namespace HyperPlatform
{
	class IWindow;
}

namespace HyperRendering
{
	class IContext;
}

namespace HyperEngine
{
	class IApplication;

	class CEngineLoop
	{
	private:
		using CreateContextFunction = HyperRendering::IContext* (*) ();

	public:
		void initialize(IApplication* application);
		void shutdown();

		void run();

	private:
		bool m_running{ false };

		IApplication* m_application{ nullptr };

		HyperPlatform::IWindow* m_window{ nullptr };
		HyperPlatform::ILibraryManager* m_library_manager{ nullptr };
		HyperPlatform::CLibraryHandle m_graphics_library{};

		HyperRendering::IContext* m_graphics_context{ nullptr };
	};
} // namespace HyperEngine
