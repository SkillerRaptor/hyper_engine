/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Application.hpp"

#include <utility>

namespace HyperEngine
{
	Application::Application(std::string title, HyperPlatform::GraphicsApi graphics_api)
		: m_title(std::move(title))
		, m_graphics_api(graphics_api)
	{
	}

	auto Application::title() const -> std::string
	{
		return m_title;
	}
	
	auto Application::graphics_api() const -> HyperPlatform::GraphicsApi
	{
		return m_graphics_api;
	}
} // namespace HyperEngine
