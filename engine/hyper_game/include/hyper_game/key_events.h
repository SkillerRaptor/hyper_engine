/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_game/key_codes.h"

#include <stdbool.h>

struct hyper_key_press_event
{
	bool repeat;
	enum hyper_key_code key_code;
};

struct hyper_key_release_event
{
	enum hyper_key_code key_code;
};

struct hyper_key_type_event
{
	enum hyper_key_code key_code;
};
