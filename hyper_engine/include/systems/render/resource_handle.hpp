/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

class ResourceHandle
{
public:
    explicit ResourceHandle(uint32_t);

    bool is_valid() const { return m_handle != 0xffffffff; }
    uint32_t handle() const { return m_handle; }

private:
    uint32_t m_handle { 0xffffffff };
};
