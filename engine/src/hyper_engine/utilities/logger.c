/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/utilities/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define FONT_RESET "\033[0m"
#define FONT_COLOR_RED "\033[30;31m"
#define FONT_COLOR_GREEN "\033[30;32m"
#define FONT_COLOR_YELLOW "\033[30;33m"
#define FONT_COLOR_CYAN "\033[30;36m"

enum logger_level
{
	LOGGER_LEVEL_INFO,
	LOGGER_LEVEL_WARN,
	LOGGER_LEVEL_ERROR,
	LOGGER_LEVEL_DEBUG,
};

static void logger_vlog(
	enum logger_level level,
	const char *format,
	va_list args);

void logger_info(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	logger_vlog(LOGGER_LEVEL_INFO, format, args);
	va_end(args);
}

void logger_warn(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	logger_vlog(LOGGER_LEVEL_WARN, format, args);
	va_end(args);
}

void logger_error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	logger_vlog(LOGGER_LEVEL_ERROR, format, args);
	va_end(args);
}

void logger_debug(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	logger_vlog(LOGGER_LEVEL_DEBUG, format, args);
	va_end(args);
}

static void logger_vlog(
	enum logger_level level,
	const char *format,
	va_list args)
{
	time_t raw_time;
	time(&raw_time);

	const struct tm *time_info = localtime(&raw_time);
	printf(
		"%02u:%02u:%02u | ",
		time_info->tm_hour,
		time_info->tm_min,
		time_info->tm_sec);

	switch (level)
	{
	case LOGGER_LEVEL_INFO:
		printf(FONT_COLOR_GREEN "info" FONT_RESET ": ");
		break;
	case LOGGER_LEVEL_WARN:
		printf(FONT_COLOR_YELLOW "warn" FONT_RESET ": ");
		break;
	case LOGGER_LEVEL_ERROR:
		printf(FONT_COLOR_RED "error" FONT_RESET ": ");
		break;
	case LOGGER_LEVEL_DEBUG:
		printf(FONT_COLOR_CYAN "debug" FONT_RESET ": ");
		break;
	}

	vprintf(format, args);
	printf("\n");
}
