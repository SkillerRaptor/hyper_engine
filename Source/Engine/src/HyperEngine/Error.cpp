/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Error.hpp"

namespace HyperEngine
{
	Error::Error(std::string string)
		: m_string(std::move(string))
	{
	}

	Error::~Error()
	{
		assert_is_checked();
	}

	Error::Error(Error &&other) noexcept
	{
		m_checked = true;
		m_string = std::move(other.m_string);

		other.m_checked = false;
		other.m_string = std::nullopt;
	}

	Error &Error::operator=(Error &&other) noexcept
	{
		assert_is_checked();

		m_checked = false;
		m_string = std::move(other.m_string);

		other.m_checked = true;
		other.m_string = std::nullopt;

		return *this;
	}

	std::string Error::string() const noexcept
	{
		return m_string.value();
	}

	bool Error::is_error()
	{
		m_checked = true;
		return m_string.has_value();
	}

	ErrorSuccess Error::success()
	{
		return {};
	}

	void Error::assert_is_checked()
	{
		if (m_checked || !m_string.has_value())
		{
			return;
		}

		std::abort();
	}
} // namespace HyperEngine
