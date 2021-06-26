/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <sstream>

namespace HyperCore
{
	class CFormatter
	{
	public:
		template <typename... Args>
		static std::string format(const std::string& format, Args&&... args)
		{
			if (format.empty())
			{
				return format;
			}

			constexpr const size_t args_count = sizeof...(Args);
			if constexpr (args_count == 0)
			{
				return format;
			}

			std::stringstream format_stream;
			uint32_t index = 0;
			for (const char c : format)
			{
				switch (c)
				{ //
				case '{':
					// TODO: Adding formatting syntax
					break;
				case '}':
				{
					format_sequence(
						index,
						format_stream,
						std::make_index_sequence<args_count>(),
						std::forward<Args&&>(args)...);
					++index;
					break;
				}
				default:
					format_stream << c;
					break;
				}
			}

			return format_stream.str();
		}

	private:
		template <typename... Args, size_t... I>
		static void format_sequence(
			uint32_t index,
			std::stringstream& format_stream,
			std::index_sequence<I...>,
			Args&&... args)
		{
			(format_character(args, index, I, format_stream), ...);
		}

		template <typename T>
		static void format_character(
			T value,
			size_t index,
			size_t current_index,
			std::stringstream& format_stream)
		{
			if (index == current_index)
			{
				format_stream << value;
			}
		}
	};
} // namespace HyperCore
