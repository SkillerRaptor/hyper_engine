/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <discord.h>

#include <cstdint>
#include <chrono>
#include <string>

namespace HyperEditor
{
	class DiscordPresence
	{
	public:
		static constexpr int64_t s_client_id = 883067332560289892;

	public:
		explicit DiscordPresence(std::string application_name);
		~DiscordPresence();

		auto initialize() -> void;

		auto update() -> void;

	private:
		std::string m_application_name;
		std::chrono::system_clock::time_point m_start_time{};
		
		bool m_successful{ false };

		discord::Core* m_core{ nullptr };
		discord::User m_user{};
	};
} // namespace HyperEditor
