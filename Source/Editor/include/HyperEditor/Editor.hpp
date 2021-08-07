/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperEngine/Application.hpp>

namespace HyperEditor
{
	class Editor : public HyperEngine::Application
	{
	public:
		Editor();
		
	private:
		virtual void startup() override;
		virtual void shutdown() override;
	};
} // namespace HyperEditor
