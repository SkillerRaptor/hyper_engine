/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <discord.h>

#include <cstdint>
#include <chrono>

namespace HyperEngine
{
	class DiscordPresence
	{
	public:
		static constexpr int64_t s_client_id = 883067332560289892;

	public:
		DiscordPresence();
		~DiscordPresence();
		
		auto initialize() -> void;

		auto update() -> void;

	private:
		std::chrono::system_clock::time_point m_start_time{};
		
		discord::Core* m_core{ nullptr };
		discord::User m_user{};
	};
} // namespace HyperEngine
