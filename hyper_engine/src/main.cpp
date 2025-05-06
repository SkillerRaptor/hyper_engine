/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#if !HE_TESTS
#    include "engine.hpp"

int main()
{
    // TODO: Parse command line arguments

    Engine engine;
    engine.initialize();
    engine.run();
    engine.shutdown();

    return 0;
}
#endif
