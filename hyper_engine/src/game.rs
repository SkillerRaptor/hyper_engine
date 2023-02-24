/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

pub trait Game {
    fn update(&mut self);

    fn render(&mut self);
}
