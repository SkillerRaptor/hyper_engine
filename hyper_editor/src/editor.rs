/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_engine::game::Game;

use std::time::Duration;

#[derive(Debug)]
pub(crate) struct Editor {}

impl Editor {
    pub fn new() -> Self {
        Self {}
    }
}

impl Game for Editor {
    fn update(&mut self, _delta_time: Duration) {}

    fn render(&mut self) {}
}
