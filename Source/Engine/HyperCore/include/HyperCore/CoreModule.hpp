/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/IModule.hpp"

namespace HyperEngine
{
	class CoreModule : public IModule
	{
	public:
		auto initialize() -> bool override;
		auto terminate() -> void override;
		
		auto name() const -> const char* override;
	};
} // namespace HyperEngine
