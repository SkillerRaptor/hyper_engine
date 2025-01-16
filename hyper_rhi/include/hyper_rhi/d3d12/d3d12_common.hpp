/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#undef near
#undef far

using Microsoft::WRL::ComPtr;

#define HE_DX_CHECK(x)                                                               \
    do                                                                               \
    {                                                                                \
        const HRESULT result = x;                                                    \
        if (FAILED(result))                                                          \
        {                                                                            \
            HE_PANIC("DirectX operation failed: {}", static_cast<uint32_t>(result)); \
        }                                                                            \
    } while (0)
