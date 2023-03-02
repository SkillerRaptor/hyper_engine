/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

mod editor;
mod logger;

use editor::Editor;
use hyper_engine::application::ApplicationBuilder;

use color_eyre::Result;

// TODO: Implement Game Trait for the engine to make it easier to communicate and adapt to the engine

fn main() -> Result<()> {
    color_eyre::install()?;

    // TODO: Implement CLI for '-debug' or '-trace' options
    logger::init(3)?;

    ApplicationBuilder::new()
        .title("HyperEditor")
        .width(1280)
        .height(720)
        .resizable(true)
        .build(Editor::new())?
        .run();

    Ok(())
}
