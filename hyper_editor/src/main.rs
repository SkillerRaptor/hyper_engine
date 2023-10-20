/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#![allow(dead_code)]
#![allow(clippy::too_many_arguments)]

mod editor;
mod logger;

use crate::editor::Editor;

use hyper_engine::application::Application;

use color_eyre::Result;
use logger::Verbosity;

fn main() -> Result<()> {
    color_eyre::install()?;

    if cfg!(debug_assertions) {
        logger::init(Verbosity::Debug)?;
    } else {
        logger::init(Verbosity::Info)?;
    }

    let mut application = Application::builder()
        .title("HyperEditor")
        .width(1280)
        .height(720)
        .resizable(true)
        .build(Box::<Editor>::default())?;

    application.run()?;

    Ok(())
}
