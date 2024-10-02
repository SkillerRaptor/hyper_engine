/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <exception>

#include <argparse/argparse.hpp>

#include <hyper_core/logger.hpp>
#include <hyper_platform/window.hpp>

int main(int argc, char** argv)
{
    argparse::ArgumentParser program("HyperEngine");

    uint32_t width = 0;
    program.add_argument("--width").default_value(static_cast<uint32_t>(1280)).scan<'i', uint32_t>().store_into(width);

    uint32_t height = 0;
    program.add_argument("--height").default_value(static_cast<uint32_t>(720)).scan<'i', uint32_t>().store_into(height);

    std::string renderer = "vulkan";
    program.add_argument("--renderer").default_value("vulkan").choices("d3d12", "vulkan").store_into(renderer);

    bool debug = false;
    program.add_argument("--debug").default_value(false).implicit_value(true).store_into(debug);

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& error)
    {
        HE_ERROR("{}", error.what());
        return 1;
    }

    hyper_platform::Window window = hyper_platform::Window({
        .title = "HyperEngine",
        .width = width,
        .height = height,
    });

    return 0;
}
