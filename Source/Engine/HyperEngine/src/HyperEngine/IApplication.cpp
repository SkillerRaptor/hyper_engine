/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/IApplication.hpp"

#include <utility>

namespace HyperEngine
{
	IApplication::IApplication(std::string title)
		: m_title(std::move(title))
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
