/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Events/IEvent.hpp>

namespace HyperCore
{
	IEvent::ECategory operator|(IEvent::ECategory left, IEvent::ECategory right)
	{
		return static_cast<IEvent::ECategory>(
			static_cast<IEvent::CategoryIndex>(left) |
			static_cast<IEvent::CategoryIndex>(right));
	}

	IEvent::EType operator|(IEvent::EType left, IEvent::EType right)
	{
		return static_cast<IEvent::EType>(
			static_cast<IEvent::TypeIndex>(left) |
			static_cast<IEvent::TypeIndex>(right));
	}
} // namespace HyperCore
