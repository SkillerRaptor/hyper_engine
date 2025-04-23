/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "engine.hpp"

int main()
{
    // TODO: Parse command line arguments

    Engine engine;
    engine.initialize();
    engine.run();
    engine.shutdown();

    return 0;
}