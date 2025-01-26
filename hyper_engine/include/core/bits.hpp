/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define HE_BIT_8(x) (static_cast<uint8_t>(1) << (x))
#define HE_BIT_16(x) (static_cast<uint16_t>(1) << (x))
#define HE_BIT_32(x) (static_cast<uint32_t>(1) << (x))
#define HE_BIT_64(x) (static_cast<uint64_t>(1) << (x))

#define HE_MASK_8(x) (BIT8(x) - static_cast<uint8_t>(1))
#define HE_MASK_16(x) (BIT16(x) - static_cast<uint16_t>(1))
#define HE_MASK_32(x) (BIT32(x) - static_cast<uint32_t>(1))
#define HE_MASK_64(x) (BIT64(x) - static_cast<uint64_t>(1))
