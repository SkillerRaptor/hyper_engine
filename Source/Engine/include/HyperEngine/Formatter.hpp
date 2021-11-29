/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <sstream>
#include <string>
#include <string_view>

namespace HyperEngine::Formatter
{
	namespace Detail
	{
		template <class T>
		concept Printable = requires(std::ostream &ostream, T value)
		{
			ostream << value;
		};

		inline void format(std::stringstream &format_stream, size_t argument_index)
		{
			HYPERENGINE_UNUSED_VARIABLE(format_stream);
			HYPERENGINE_UNUSED_VARIABLE(argument_index);
		}

		template <Printable T, Printable... Args>
		void format(
			std::stringstream &format_stream,
			size_t argument_index,
			T &&argument,
			Args &&...args)
		{
			if (argument_index != 0)
			{
				format(format_stream, argument_index - 1, std::forward<Args>(args)...);
				return;
			}

			format_stream << argument;
		}
	} // namespace Detail

	template <Detail::Printable... Args>
	Expected<std::string> format(std::string_view format, Args &&...args)
	{
		std::stringstream format_stream;

		size_t argument_index = 0;
		for (auto it = format.begin(); it != format.end(); ++it)
		{
			if (*it != '{')
			{
				format_stream << *it;
				continue;
			}

			++it;

			if (*it != '}')
			{
				return Error("bad format string");
			}

			if (argument_index >= sizeof...(args))
			{
				return Error("bad format string");
			}

			Detail::format(
				format_stream, argument_index++, std::forward<Args>(args)...);
		}

		return format_stream.str();
	}
} // namespace HyperEngine::Formatter
