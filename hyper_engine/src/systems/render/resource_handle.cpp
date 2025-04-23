/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "systems/render/resource_handle.hpp"

ResourceHandle::ResourceHandle()
    : m_handle(0xffffffff)
{
}

ResourceHandle::ResourceHandle(const uint32_t handle)
    : m_handle(handle)
{
}

bool ResourceHandle::is_valid() const
{
    return m_handle != 0xffffffff;
}

uint32_t ResourceHandle::handle() const
{
    return m_handle;
}