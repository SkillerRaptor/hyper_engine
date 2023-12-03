/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

mod editor;
mod logger;

use crate::editor::Editor;

use hyper_engine::application::{Application, ApplicationDescriptor};

use color_eyre::Result;
use logger::Verbosity;

fn main() -> Result<()> {
    color_eyre::install()?;

    if cfg!(debug_assertions) {
        logger::init(Verbosity::Debug)?;
    } else {
        logger::init(Verbosity::Info)?;
    }

    let mut application = Application::new(
        Box::<Editor>::default(),
        &ApplicationDescriptor {
            title: "HyperEditor".to_owned(),
            width: 1280,
            height: 720,
            resizable: true,
        },
    )?;

    application.run()?;

    Ok(())
}
