/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Application.hpp"

namespace HyperEngine
{
	Application::Application(const std::string& title)
		: m_title(title)
	{
	}
	
	std::string Application::title() const
	{
		return m_title;
	}
}
