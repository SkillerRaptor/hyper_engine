/*
 * Copyright (c) 2022-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use clap::{Parser, ValueEnum};

use hyper_rhi::graphics_device::GraphicsApi;

use crate::engine::{Engine, EngineDescriptor};

mod engine;

#[derive(Clone, Copy, Debug, ValueEnum)]
enum Renderer {
    D3D12,
    Vulkan,
}

#[derive(Debug, Parser)]
struct Arguments {
    /// Width of the window
    #[arg(long, default_value_t = 1280)]
    width: u32,

    /// Height of the window
    #[arg(long, default_value_t = 720)]
    height: u32,

    /// Renderer API of the engine
    #[arg(long, value_enum, default_value_t = Renderer::Vulkan)]
    renderer: Renderer,

    /// Enables renderer debug mode
    #[arg(long, default_value_t = false)]
    debug: bool,
}

fn main() {
    let arguments = Arguments::parse();

    let mut engine = Engine::new(&EngineDescriptor {
        width: arguments.width,
        height: arguments.height,
        graphics_api: match arguments.renderer {
            Renderer::D3D12 => GraphicsApi::D3D12,
            Renderer::Vulkan => GraphicsApi::Vulkan,
        },
        debug: arguments.debug,
    });

    engine.run();
}
