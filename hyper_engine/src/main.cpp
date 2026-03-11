/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <hyper_core/logger.hpp>

#include "engine.hpp"

int main()
{
#if HE_DEBUG_BUILD
    he::logger::initialize(he::logger::Level::Debug);
#else
    he::logger::initialize(he::logger::Level::Info);
#endif

    // TODO: Parse command line arguments

    he::Engine engine;
    engine.run();

    return EXIT_SUCCESS;
}
