/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string>

namespace HyperEngine
{
	class ErrorSuccess;

	class Error
	{
	public:
		explicit Error(std::string string);
		~Error();

		Error(const Error &other) = delete;
		Error(Error &&other) noexcept;

		Error &operator=(const Error &other) = delete;
		Error &operator=(Error &&other) noexcept;

		std::string string() const noexcept;

		bool is_error();

		static ErrorSuccess success();

	protected:
		Error() = default;

	private:
		void assert_is_checked();

	private:
		bool m_checked = false;
		std::optional<std::string> m_string = {};
	};

	class ErrorSuccess final : public Error
	{
	};
} // namespace HyperEngine
