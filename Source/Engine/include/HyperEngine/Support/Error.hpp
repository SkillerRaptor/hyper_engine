/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <ostream>
#include <string>

namespace HyperEngine
{
	class ErrorSuccess;

	class Error
	{
	public:
		explicit Error(std::string error);

		std::string error() const;

		bool is_error() const noexcept;

		operator bool() const noexcept;

		static ErrorSuccess success();

	protected:
		Error() = default;

	private:
		std::optional<std::string> m_error = {};
	};

	class ErrorSuccess final : public Error
	{
	};

	std::ostream &operator<<(std::ostream &ostream, const Error &error);
} // namespace HyperEngine
