/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/GraphicsApi.hpp>

#include <string>

namespace HyperEngine
{
	class IApplication
	{
	public:
		IApplication(std::string title, HyperPlatform::GraphicsApi t_graphics_api = HyperPlatform::GraphicsApi::OpenGL);
		virtual ~IApplication() = default;

		virtual auto startup() -> void;
		virtual auto shutdown() -> void;

		virtual auto update() -> void;

		auto title() const -> std::string;
		auto graphics_api() const -> HyperPlatform::GraphicsApi;

	private:
		std::string m_title;
		HyperPlatform::GraphicsApi m_graphics_api{ HyperPlatform::GraphicsApi::OpenGL };
	};
} // namespace HyperEngine
