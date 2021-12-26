/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Support/Error.hpp"

namespace HyperEngine
{
	Error::Error(std::string error)
		: m_error(std::move(error))
	{
	}

	std::string Error::error() const
	{
		return m_error;
	}
} // namespace HyperEngine
