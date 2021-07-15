/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Prerequisites.hpp>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace HyperCore
{
	class CFormatter
	{
	public:
		HYPERENGINE_NON_COPYABLE(CFormatter);
		HYPERENGINE_NON_MOVEABLE(CFormatter);
		
	public:
		constexpr CFormatter() = delete;
		~CFormatter() = delete;
		
		template <typename... Args>
		static std::string format(std::string_view format, Args&&... args)
		{
			if (format.empty())
			{
				return format.data();
			}

			constexpr const size_t args_count = sizeof...(Args);
			if constexpr (args_count == 0)
			{
				return format.data();
			}

			uint32_t argument_index = 0;
			std::stringstream format_stream;
			
			for (size_t i = 0; i < format.size(); ++i)
			{
				switch (format[i])
				{
				case '{':
					break;
				case '}':
				{
					format_sequence(
						argument_index,
						format_stream,
						std::make_index_sequence<args_count>(),
						std::forward<Args>(args)...);
					++argument_index;
					break;
				}
				default:
					format_stream << format[i];
					break;
				}
			}

			return format_stream.str();
		}

	private:
		template <size_t... I, typename... Args>
		static void format_sequence(
			uint32_t argument_index,
			std::stringstream& format_stream,
			std::index_sequence<I...>,
			Args&&... args)
		{
			(format_character(argument_index, I, format_stream, args), ...);
		}

		template <typename T>
		static void format_character(
			size_t argument_index,
			size_t current_argument_index,
			std::stringstream& format_stream,
			T value)
		{
			if (argument_index == current_argument_index)
			{
				format_stream << value;
			}
		}
	};
} // namespace HyperCore
