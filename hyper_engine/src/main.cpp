/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "../include/engine_loop.hpp"

int main(const int argc, const char **argv)
{
    hyper_engine::EngineLoop engine_loop;
    if (!engine_loop.pre_initialize(argc, argv))
    {
        return 1;
    }

    if (!engine_loop.initialize())
    {
        return 1;
    }

    engine_loop.run();

    return 0;
}
