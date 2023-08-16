/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use egui::Context;

pub trait Game {
    fn update(&mut self);

    fn update_fixed(&mut self, delta_time: f32, time: f32);

    fn render(&mut self);

    fn render_gui(&mut self, context: &Context);
}
