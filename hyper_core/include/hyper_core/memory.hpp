/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

namespace he
{
    template <typename T, typename Deleter = std::default_delete<T>>
    using OwnPtr = std::unique_ptr<T, Deleter>;

    template <typename T, typename... Args>
    OwnPtr<T> make_own(Args &&...args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    OwnPtr<T> wrap_own(T *ptr)
    {
        return OwnPtr<T>(ptr);
    }

    template <typename T>
    using RefPtr = std::shared_ptr<T>;

    template <typename T, typename... Args>
    RefPtr<T> make_ref(Args &&...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    RefPtr<T> wrap_ref(T *ptr)
    {
        return RefPtr<T>(ptr);
    }

    template <typename T, typename U>
    RefPtr<T> cast_ref(const RefPtr<U> &ptr)
    {
        return std::static_pointer_cast<T>(ptr);
    }
} // namespace he
