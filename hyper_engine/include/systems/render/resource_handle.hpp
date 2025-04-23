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
    ResourceHandle();
    explicit ResourceHandle(uint32_t handle);

    bool is_valid() const;
    uint32_t handle() const;

private:
    uint32_t m_handle;
};