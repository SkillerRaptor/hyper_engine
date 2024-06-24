/*
 * Copyright (c) 2022-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

mod editor;
mod logger;

use std::num::NonZeroU32;

use anyhow::{Context, Result};
use hyper_engine::{Application, ApplicationDescriptor};
use logger::Verbosity;

use crate::editor::Editor;

fn main() -> Result<()> {
    if cfg!(debug_assertions) {
        logger::init(Verbosity::Debug)?;
    } else {
        logger::init(Verbosity::Info)?;
    }

    // TODO: Pass editor size from config?
    let mut application = Application::new(
        Box::<Editor>::default(),
        ApplicationDescriptor {
            title: "HyperEditor",
            width: NonZeroU32::new(1280).context("invalid width, expected non-zero value")?,
            height: NonZeroU32::new(720).context("invalid height, expected non-zero value")?,
            resizable: true,
        },
    )?;

    application.run();

    Ok(())
}
