//
// Copyright (c) 2022-024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

mod engine;

use clap::{Parser, ValueEnum};

use hyper_rhi::GraphicsApi;
use tracing::level_filters::LevelFilter;
use tracing_subscriber::{
    filter,
    layer::{Layer, SubscriberExt},
    util::SubscriberInitExt,
};

use crate::engine::{Engine, EngineDescriptor};

#[derive(Clone, Copy, Debug, ValueEnum)]
enum Renderer {
    D3D12,
    Vulkan,
}

#[derive(Debug, Parser)]
struct Arguments {
    /// Enables logging mode
    #[arg(long, default_value_t = false)]
    log: bool,

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

    if arguments.log {
        let filter = filter::filter_fn(|metadata| metadata.target().starts_with("hyper_"));

        let fmt_layer = tracing_subscriber::fmt::layer()
            .with_thread_names(true)
            .with_filter(LevelFilter::TRACE)
            .with_filter(filter);

        tracing_subscriber::registry().with(fmt_layer).init();
    }

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
