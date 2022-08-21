/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_engine::application;

fn main() -> Result<(), String> {
    let mut application = application::Application::new();
    application.run();

    Ok(())
}
