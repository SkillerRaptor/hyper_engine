/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;

use color_eyre::eyre::Result;

pub struct GraphicsContext {}

impl GraphicsContext {
    pub fn new(_window: &Window) -> Result<Self> {
        Ok(Self {})
    }
}
