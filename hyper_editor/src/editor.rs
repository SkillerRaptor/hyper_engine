/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_engine::game::Game;

use std::time::Duration;

/// A struct representing the game engines editor interface
#[derive(Debug)]
pub(crate) struct Editor {}

impl Editor {
    /// Constructs a new editor struct
    pub(crate) fn new() -> Self {
        Self {}
    }
}

impl Game for Editor {
    fn update(&mut self, _delta_time: Duration) {}

    fn render(&mut self) {}
}
