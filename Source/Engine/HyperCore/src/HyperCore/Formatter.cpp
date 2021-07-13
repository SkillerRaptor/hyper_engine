/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Formatter.hpp>
#include <HyperCore/Logger.hpp>

namespace HyperCore
{
	void CFormatter::internal_warning(std::string_view string)
	{
		CLogger::warning(string);
	}
}
