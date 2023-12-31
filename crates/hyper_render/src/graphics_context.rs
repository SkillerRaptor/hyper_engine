/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;
use hyper_vulkan::instance::{Instance, InstanceDescriptor};

use color_eyre::eyre::Result;

pub struct GraphicsContext {}

impl GraphicsContext {
    pub fn new(window: &Window) -> Result<Self> {
        const DEBUG_MODE: bool = cfg!(debug_assertions);

        let instance = Instance::new(
            window,
            InstanceDescriptor {
                validation_layers: DEBUG_MODE,
            },
        )?;

        let _surface = instance.create_surface(window)?;

        Ok(Self {})
    }
}
