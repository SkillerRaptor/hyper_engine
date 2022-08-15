/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

extern crate hyper_engine;

use hyper_engine::core;

fn main() {
    let application = core::application::Application::new();
    application.run();
}
