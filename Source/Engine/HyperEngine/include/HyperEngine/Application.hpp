/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

namespace HyperEngine
{
	class Application
	{
	public:
		friend class Launcher;

	public:
		explicit Application(std::string title);
		virtual ~Application() = default;
		
		std::string title() const;

	protected:
		virtual void startup() = 0;
		virtual void shutdown() = 0;

	private:
		std::string m_title;
	};
} // namespace HyperEngine
