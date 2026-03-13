/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/memory.hpp>

namespace he {

class Resource {
protected:
    explicit Resource(RefPtr<void> internal_state)
        : m_internal_state(std::move(internal_state))
    {
    }

    const void *internal_state() const { return m_internal_state.get(); }

private:
    RefPtr<void> m_internal_state { nullptr };
};

} // namespace he
