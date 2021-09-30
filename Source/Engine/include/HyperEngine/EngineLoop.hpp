/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

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
		bool m_running{ false };
	};
} // namespace HyperEngine
