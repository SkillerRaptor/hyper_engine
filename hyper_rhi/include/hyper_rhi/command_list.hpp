/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

namespace hyper_rhi
{
    class CommandList
    {
    public:
        virtual ~CommandList() = default;

        virtual void begin() const = 0;
        virtual void end() const = 0;
    };

    using CommandListHandle = std::shared_ptr<CommandList>;
} // namespace hyper_rhi
