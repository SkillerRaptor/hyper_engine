/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperGame/EventSystem/EventManager.hpp"
#include "HyperGame/EntitySystem/Registry.hpp"

#include <HyperCore/IModule.hpp>

namespace HyperEngine
{
	class GameModule : public IModule
	{
	public:
		auto initialize() -> bool override;
		auto terminate() -> void override;
		
		auto name() const -> const char* override;

	private:
		EventManager m_event_manager{};
		Registry m_registry{};
	};
} // namespace HyperEngine
