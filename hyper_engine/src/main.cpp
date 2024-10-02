/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/window.hpp"

int main()
{
    hyper_platform::Window window = hyper_platform::Window({
        .title = "HyperEngine",
        .width = 1280,
        .height = 720,
    });
}
