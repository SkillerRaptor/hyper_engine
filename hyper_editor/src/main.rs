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
    logger::init(3)?; // Using hardcoded value until cli parsing is added

    let mut application = Application::new()?;
    application.run()?;

    Ok(())
}
