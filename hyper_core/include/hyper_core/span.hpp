/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <span>

namespace he {

template <typename T>
using Span = std::span<T>;

template <typename T>
using ReadonlySpan = std::span<const T>;

} // namespace he
