/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Error.hpp"

#include <cassert>

namespace HyperEngine
{
	Error::Error(std::string error)
		: m_error(std::move(error))
	{
	}

	std::string Error::error() const
	{
		assert(m_error.has_value());
		return m_error.value();
	}

	bool Error::is_error() const noexcept
	{
		return m_error.has_value();
	}

	Error::operator bool() const noexcept
	{
		return m_error.has_value();
	}

	ErrorSuccess Error::success()
	{
		return {};
	}

	std::ostream &operator<<(std::ostream &ostream, const Error &error)
	{
		ostream << error.error();
		return ostream;
	}
} // namespace HyperEngine
