/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/debug.h"
#include "hyper_common/source_location.h"

void hyper_logger_info(
	struct hyper_source_location source_location,
	const char *format,
	...);
void hyper_logger_warning(
	struct hyper_source_location source_location,
	const char *format,
	...);
void hyper_logger_error(
	struct hyper_source_location source_location,
	const char *format,
	...);
void hyper_logger_debug(
	struct hyper_source_location source_location,
	const char *format,
	...);

#define hyper_logger_info$(...) \
	hyper_logger_info(hyper_source_location_current$(), __VA_ARGS__)
#define hyper_logger_warning$(...) \
	hyper_logger_warning(hyper_source_location_current$(), __VA_ARGS__)
#define hyper_logger_error$(...) \
	hyper_logger_error(hyper_source_location_current$(), __VA_ARGS__)
#if HYPER_DEBUG
#	define hyper_logger_debug$(...) \
		hyper_logger_debug(hyper_source_location_current$(), __VA_ARGS__)
#else
#	define hyper_logger_debug$(...) ((void) 0)
#endif
