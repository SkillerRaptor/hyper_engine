/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

mod logger;

use hyper_engine::application::Application;

use color_eyre::Result;

// TODO: Implement Game Trait for the engine to make it easier to communicate and adapt to the engine

fn main() -> Result<()> {
    color_eyre::install()?;

    // TODO: Implement CLI for '-debug' or '-trace' options
    logger::init(3)?;

    // FIXME: Don't pack everything into the Application class to improve flexibility
    let mut application = Application::new()?;
    application.run();
}
