/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Events/EventManager.hpp>

namespace HyperCore
{
	CEventFamilyGenerator::EventIdType CEventFamilyGenerator::identifier()
	{
		static EventIdType identifier = 0;
		return identifier++;
	}
} // namespace HyperCore
