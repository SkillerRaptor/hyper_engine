/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperEngine/Application.hpp>

namespace HyperEditor
{
	class Editor final : public HyperEngine::Application
	{
	public:
		Editor();
		
	private:
		virtual auto startup() -> void override;
		virtual auto shutdown() -> void override;
	};
} // namespace HyperEditor
