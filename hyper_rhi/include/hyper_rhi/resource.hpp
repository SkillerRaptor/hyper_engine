/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

namespace he {

class Resource {
protected:
    explicit Resource(std::shared_ptr<void> internal_state)
        : m_internal_state(std::move(internal_state))
    {
    }

    const void *internal_state() const { return m_internal_state.get(); }

private:
    std::shared_ptr<void> m_internal_state = nullptr;
};

} // namespace he
