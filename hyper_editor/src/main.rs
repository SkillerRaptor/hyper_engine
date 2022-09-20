/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_engine::application::Application;

use color_eyre::Result;

fn main() -> Result<()> {
    color_eyre::install()?;

    let mut application = Application::new()?;
    application.run()?;

    Ok(())
}
