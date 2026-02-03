/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "core/logger.hpp"
#include "engine.hpp"

using namespace he;

int main()
{
#if HE_DEBUG_BUILD
    logger::initialize(spdlog::level::debug);
#else
    logger::initialize(spdlog::level::info);
#endif

    // TODO: Parse command line arguments

    std::optional<Engine> engine = Engine::create();
    if (!engine.has_value())
    {
        return EXIT_FAILURE;
    }

    engine->run();

    return EXIT_SUCCESS;
}
