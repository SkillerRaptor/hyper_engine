/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/IApplication.hpp"

#include <utility>

namespace HyperEngine
{
	IApplication::IApplication(std::string title, HyperPlatform::GraphicsApi graphics_api)
		: m_title(std::move(title))
		, m_graphics_api(graphics_api)
	{
	}

	auto IApplication::title() const -> std::string
	{
		return m_title;
	}
	
	auto IApplication::graphics_api() const -> HyperPlatform::GraphicsApi
	{
		return m_graphics_api;
	}
} // namespace HyperEngine
