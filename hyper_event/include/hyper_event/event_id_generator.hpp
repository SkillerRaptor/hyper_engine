/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace hyper_event
{
    class EventIdGenerator
    {
    public:
        template <typename>
        [[nodiscard]] static size_t type()
        {
            static const size_t value = EventIdGenerator::identifier();
            return value;
        }

    private:
        static size_t identifier()
        {
            static size_t value = 0;
            return value++;
        }
    };
} // namespace hyper_event
