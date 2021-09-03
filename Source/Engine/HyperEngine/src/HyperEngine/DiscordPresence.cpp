/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/DiscordPresence.hpp"

#include <HyperCore/Logger.hpp>

namespace HyperEngine
{
	DiscordPresence::DiscordPresence()
		: m_start_time(std::chrono::system_clock::now())
	{
	}

	DiscordPresence::~DiscordPresence()
	{
		auto success = true;
		if (m_core != nullptr)
		{
			delete m_core;
		}
		else
		{
			success = false;
		}

		if (success)
		{
			HyperCore::Logger::info("Successfully destroyed discord presence");
		}
	}

	auto DiscordPresence::initialize() -> void
	{
		if (discord::Core::Create(s_client_id, DiscordCreateFlags_NoRequireDiscord, &m_core) != discord::Result::Ok)
		{
			HyperCore::Logger::warning("Cannot instantiate discord core");
			return;
		}

		m_core->SetLogHook(
			discord::LogLevel::Debug,
			[](discord::LogLevel level, const char* message)
			{
				switch (level)
				{
				case discord::LogLevel::Error:
					HyperCore::Logger::error("DiscordPresence::LogHook(): {}", message);
					break;
				case discord::LogLevel::Warn:
					HyperCore::Logger::warning("DiscordPresence::LogHook(): {}", message);
					break;
				case discord::LogLevel::Info:
					HyperCore::Logger::info("DiscordPresence::LogHook(): {}", message);
					break;
				case discord::LogLevel::Debug:
					HyperCore::Logger::debug("DiscordPresence::LogHook(): {}", message);
					break;
				default:
					break;
				}
			});

		m_core->UserManager().OnCurrentUserUpdate.Connect(
			[this]()
			{
				m_core->UserManager().GetCurrentUser(&m_user);
				HyperCore::Logger::info("Successfully connected to user {}#{}", m_user.GetUsername(), m_user.GetDiscriminator());
			});
		
		discord::Activity activity{};
		activity.SetDetails("No Scene");
		activity.SetType(discord::ActivityType::Playing);
		activity.SetInstance(false);
		activity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(m_start_time.time_since_epoch()).count());
		activity.GetAssets().SetLargeImage("logo");
		activity.GetAssets().SetLargeText("HyperEngine");
		m_core->ActivityManager().UpdateActivity(
			activity,
			[](discord::Result result)
			{
				if (result == discord::Result::Ok)
				{
					HyperCore::Logger::info("Successfully updated activity");
				}
				else
				{
					HyperCore::Logger::error("Failed to update activity");
				}
			});

		HyperCore::Logger::info("Successfully instantiated discord presence");
	}

	auto DiscordPresence::update() -> void
	{
		m_core->RunCallbacks();
	}
} // namespace HyperEngine
