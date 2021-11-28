/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/Support/Prerequisites.hpp"

namespace HyperEngine
{
	EngineLoop::EngineLoop(Application &application, Error &error)
		: m_application(application)
	{
		HYPERENGINE_UNUSED_VARIABLE(m_application);
		HYPERENGINE_UNUSED_VARIABLE(error);
	}

	void EngineLoop::run()
	{
		while (true)
		{
			break;
		}
	}

	Expected<EngineLoop> EngineLoop::create(Application &application)
	{
		Error error = Error::success();
		EngineLoop engine_loop(application, error);
		if (error.is_error())
		{
			return error;
		}

		return engine_loop;
	}
} // namespace HyperEngine
