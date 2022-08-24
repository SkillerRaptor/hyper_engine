/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_engine::application;

fn main() {
    let mut application = match application::Application::new() {
        Some(application) => application,
        None => return,
    };

    application.run();
}
