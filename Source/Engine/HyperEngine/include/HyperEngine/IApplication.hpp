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
		friend class Launcher;

	public:
		explicit IApplication(std::string title);
		virtual ~IApplication() = default;

		auto set_title(std::string title) -> void;
		auto title() const -> std::string;
		
		auto set_graphics_api(HyperPlatform::GraphicsApi graphics_api) -> void;
		auto graphics_api() const -> HyperPlatform::GraphicsApi;

	protected:
		virtual auto startup() -> void = 0;
		virtual auto shutdown() -> void = 0;

	private:
		std::string m_title;
		HyperPlatform::GraphicsApi m_graphics_api{ HyperPlatform::GraphicsApi::OpenGL33 };
	};
} // namespace HyperEngine
