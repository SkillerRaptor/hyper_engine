/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <hyper_core/logger.hpp>

#include "engine.hpp"

using namespace he;

int main()
{
#if HE_DEBUG_BUILD
    logger::initialize(logger::Level::Debug);
#else
    logger::initialize(logger::Level::Info);
#endif

    // TODO: Parse command line arguments

    Engine engine;
    engine.run();

    return EXIT_SUCCESS;
}
