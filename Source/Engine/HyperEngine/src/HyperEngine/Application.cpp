/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Application.hpp"

#include <utility>

namespace HyperEngine
{
	Application::Application(std::string title)
		: m_title(std::move(title))
	{
	}
	
	std::string Application::title() const
	{
		return m_title;
	}
}
