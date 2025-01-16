/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/ref_ptr.hpp>

namespace hyper_engine
{
    class Model;

    struct ModelComponent
    {
        std::shared_ptr<Model> model;
    };
} // namespace hyper_engine