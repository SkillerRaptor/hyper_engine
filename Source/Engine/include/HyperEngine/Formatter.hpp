/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Concepts.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <sstream>
#include <string>
#include <string_view>

namespace HyperEngine::Formatter
{
	namespace Detail
	{
		inline void format(std::stringstream &, size_t)
		{
		}

		template <Printable T, Printable... Args>
		void format(
			std::stringstream &stream,
			size_t argument_index,
			T &&argument,
			Args &&...args)
		{
			if (argument_index != 0)
			{
				format(stream, argument_index - 1, std::forward<Args>(args)...);
				return;
			}

			stream << argument;
		}
	} // namespace Detail

	template <Printable... Args>
	Expected<std::string> format(std::string_view format, Args &&...args)
	{
		std::stringstream stream;

		size_t argument_index = 0;
		constexpr size_t argument_count = sizeof...(args);
		for (auto it = format.cbegin(); it != format.cend(); ++it)
		{
			if (*it != '{')
			{
				stream << *it;
				continue;
			}

			++it;

			if (*it != '}')
			{
				return Error("format bracket unclosed");
			}

			if (argument_index >= argument_count)
			{
				return Error("format argument not found");
			}

			Detail::format(stream, argument_index++, std::forward<Args>(args)...);
		}

		return stream.str();
	}
} // namespace HyperEngine::Formatter
