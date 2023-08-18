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

use hyper_engine::application::ApplicationBuilder;

use logger::Verbosity;

fn main() {
    color_eyre::install().unwrap();

    if cfg!(debug_assertions) {
        logger::init(Verbosity::Debug);
    } else {
        logger::init(Verbosity::Info);
    }

    ApplicationBuilder::new()
        .title("HyperEditor")
        .width(1280)
        .height(720)
        .resizable(true)
        .build(Box::new(Editor::new()))
        .run();
}
