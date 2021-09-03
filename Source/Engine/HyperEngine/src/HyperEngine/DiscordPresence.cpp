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
		auto destroy_core = [this]() -> bool
		{
			if (m_core == nullptr)
			{
				return false;
			}

			delete m_core;
			HyperCore::Logger::debug("Discord core was destroyed");

			return true;
		};

		if (!destroy_core())
		{
			return;
		}

		HyperCore::Logger::info("Successfully destroyed discord presence");
	}

	auto DiscordPresence::initialize() -> void
	{
		if (discord::Core::Create(s_client_id, DiscordCreateFlags_NoRequireDiscord, &m_core) != discord::Result::Ok)
		{
			HyperCore::Logger::warning("Cannot instantiate discord core");
			return;
		}

		HyperCore::Logger::debug("Discord core was instantiated");

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
				HYPERENGINE_VARIABLE_NOT_USED(result);
			});

		m_successful = true;

		HyperCore::Logger::info("Successfully instantiated discord presence");
	}

	auto DiscordPresence::update() -> void
	{
		if (m_successful)
		{
			m_core->RunCallbacks();
		}
	}
} // namespace HyperEngine
