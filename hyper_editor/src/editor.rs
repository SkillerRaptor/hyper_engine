/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_engine::game::Game;

pub(crate) struct Editor {}

impl Editor {
    pub(crate) fn new() -> Self {
        Self {}
    }
}

impl Game for Editor {
    fn update(&mut self) {}

    fn update_fixed(&mut self, _delta_time: f32, _time: f32) {}

    fn render(&mut self) {}
}
