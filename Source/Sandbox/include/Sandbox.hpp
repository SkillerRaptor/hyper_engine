/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperEngine/IApplication.hpp>

class Sandbox final : public HyperEngine::IApplication
{
public:
	Sandbox();

private:
	auto startup() -> void override;
	auto shutdown() -> void override;
};
