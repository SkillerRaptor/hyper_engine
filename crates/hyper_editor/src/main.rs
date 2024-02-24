/*
 * Copyright (c) 2022-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

mod editor;
mod logger;

use color_eyre::Result;
use hyper_engine::application::{Application, ApplicationDescriptor};
use logger::Verbosity;

use crate::editor::Editor;

fn main() -> Result<()> {
    color_eyre::install()?;

    if cfg!(debug_assertions) {
        logger::init(Verbosity::Debug)?;
    } else {
        logger::init(Verbosity::Info)?;
    }

    let mut application = Application::new(
        Box::<Editor>::default(),
        ApplicationDescriptor {
            title: "HyperEditor",
            width: 1280,
            height: 720,
            resizable: true,
        },
    )?;

    application.run()?;

    Ok(())
}
