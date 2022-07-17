/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_common/logger.h"

#include "hyper_common/file.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define HYPER_FONT_RESET "\033[0m"
#define HYPER_FONT_COLOR_RED "\033[30;31m"
#define HYPER_FONT_COLOR_GREEN "\033[30;32m"
#define HYPER_FONT_COLOR_YELLOW "\033[30;33m"
#define HYPER_FONT_COLOR_CYAN "\033[30;36m"

enum hyper_logger_level
{
	HYPER_LOGGER_LEVEL_INFO,
	HYPER_LOGGER_LEVEL_WARNING,
	HYPER_LOGGER_LEVEL_ERROR,
	HYPER_LOGGER_LEVEL_DEBUG,
};

static void hyper_logger_vlog(
	enum hyper_logger_level level,
	struct hyper_source_location source_location,
	const char *format,
	va_list args)
{
	time_t raw_time;
	time(&raw_time);

	const struct tm *time_info = localtime(&raw_time);
	printf(
		"%02u:%02u:%02u | %s:%u: ",
		time_info->tm_hour,
		time_info->tm_min,
		time_info->tm_sec,
		hyper_strip_file_name$(source_location.file_name),
		source_location.line);

	switch (level)
	{
	case HYPER_LOGGER_LEVEL_INFO:
		printf(HYPER_FONT_COLOR_GREEN "info" HYPER_FONT_RESET ": ");
		break;
	case HYPER_LOGGER_LEVEL_WARNING:
		printf(HYPER_FONT_COLOR_YELLOW "warning" HYPER_FONT_RESET ": ");
		break;
	case HYPER_LOGGER_LEVEL_ERROR:
		printf(HYPER_FONT_COLOR_RED "error" HYPER_FONT_RESET ": ");
		break;
	case HYPER_LOGGER_LEVEL_DEBUG:
		printf(HYPER_FONT_COLOR_CYAN "debug" HYPER_FONT_RESET ": ");
		break;
	}

	vprintf(format, args);
}

void hyper_logger_info(
	struct hyper_source_location source_location,
	const char *format,
	...)
{
	va_list args;
	va_start(args, format);
	hyper_logger_vlog(HYPER_LOGGER_LEVEL_INFO, source_location, format, args);
	va_end(args);
}

void hyper_logger_warning(
	struct hyper_source_location source_location,
	const char *format,
	...)
{
	va_list args;
	va_start(args, format);
	hyper_logger_vlog(HYPER_LOGGER_LEVEL_WARNING, source_location, format, args);
	va_end(args);
}

void hyper_logger_error(
	struct hyper_source_location source_location,
	const char *format,
	...)
{
	va_list args;
	va_start(args, format);
	hyper_logger_vlog(HYPER_LOGGER_LEVEL_ERROR, source_location, format, args);
	va_end(args);
}

void hyper_logger_debug(
	struct hyper_source_location source_location,
	const char *format,
	...)
{
	va_list args;
	va_start(args, format);
	hyper_logger_vlog(HYPER_LOGGER_LEVEL_DEBUG, source_location, format, args);
	va_end(args);
}
