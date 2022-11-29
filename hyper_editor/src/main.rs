/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

mod logger;

use hyper_engine::application::Application;

use color_eyre::Result;

fn main() -> Result<()> {
    color_eyre::install()?;

    // We are using a hardcoded value for the logger
    // until we have a CLI to parse it
    logger::init(3)?;

    let mut application = Application::new()?;
    application.run()?;

    Ok(())
}
