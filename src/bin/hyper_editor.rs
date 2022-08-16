/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

extern crate hyper_engine;

use hyper_engine::core;

fn main() -> Result<(), String> {
    let mut application = core::application::Application::new();
    application.run();

    Ok(())
}
