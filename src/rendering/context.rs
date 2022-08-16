/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::core::window::Window;
use crate::rendering::device::Device;
use crate::rendering::error::Error;
use crate::rendering::instance::Instance;
use crate::rendering::surface::Surface;

use log::info;

pub struct RenderContext {
    _device: Device,
    _surface: Surface,
    _instance: Instance,
    _entry: ash::Entry,
}

impl RenderContext {
    pub fn new(window: &Window) -> Result<Self, Error> {
        let entry = unsafe { ash::Entry::load()? };
        let instance = Instance::new(&window, &entry)?;
        let surface = Surface::new(&window, &entry, &instance)?;
        let device = Device::new(&instance, &surface)?;

        info!("Created render context");
        Ok(Self {
            _device: device,
            _surface: surface,
            _instance: instance,
            _entry: entry,
        })
    }
}
