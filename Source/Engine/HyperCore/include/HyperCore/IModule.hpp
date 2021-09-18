/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperEngine
{
	class ModuleManager;

	class IModule
	{
	public:
		friend class ModuleManager;

	public:
		virtual ~IModule() = default;

		virtual auto initialize() -> bool;
		virtual auto terminate() -> void;
		
		virtual auto update() -> void;
		
		virtual auto name() const -> const char* = 0;

	protected:
		ModuleManager* m_module_manager{ nullptr };
	};
} // namespace HyperEngine
