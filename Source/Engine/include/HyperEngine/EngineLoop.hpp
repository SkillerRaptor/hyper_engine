/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Platform/Window.hpp"

namespace HyperEngine
{
	class IApplication;

	class CEngineLoop
	{
	public:
		struct SDescription
		{
			IApplication* application;
		};

	public:
		auto create(const SDescription& description) -> bool;

		auto run() -> void;

	private:
		IApplication* m_application{ nullptr };
		
		CWindow m_window{};
		
		bool m_running{ false };
	};
} // namespace HyperEngine
