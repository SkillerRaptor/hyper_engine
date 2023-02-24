/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::time::Duration;

pub trait Game {
    fn update(&mut self, delta_time: Duration);

    fn render(&mut self);
}
