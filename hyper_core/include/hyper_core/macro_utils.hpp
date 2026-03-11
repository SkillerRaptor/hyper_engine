/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_core/types.hpp"

#define HE_MACRO_SIZE(...) ::he::detail::argument_count(__VA_ARGS__)

namespace he::detail {

template <typename... Args>
static constexpr usize argument_count(Args &&...)
{
    return sizeof...(Args);
}

} // namespace he::detail
