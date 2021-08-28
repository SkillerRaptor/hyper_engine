/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperPlatform/GraphicsApi.hpp"

#include <HyperCore/Result.hpp>
#include <HyperCore/Events/EventManager.hpp>

#include <cstdint>
#include <string>

struct GLFWwindow;

namespace HyperPlatform
{
	struct WindowInfo
	{
		std::string title;
		HyperCore::EventManager& event_manager;
		GraphicsApi graphics_api;
	};
	
	class Window
	{
	private:
		struct Info
		{
			std::string title;
			int width{ 0 };
			int height{ 0 };
			HyperCore::EventManager* event_manager{ nullptr };
		};

	public:
		Window(const WindowInfo& t_window_info);
		~Window();

		auto initialize() -> HyperCore::InitializeResult;

		auto poll_events() const -> void;
		
		auto time() const -> float;
		auto title() const -> std::string;
		auto width() const -> int;
		auto height() const -> int;
		auto event_manager() -> HyperCore::EventManager*;
		auto native_window() const -> GLFWwindow*;

	private:
		Info m_info{};
		GraphicsApi m_graphics_api{ GraphicsApi::Vulkan };

		GLFWwindow* m_native_window{ nullptr };
	};
} // namespace HyperPlatform
