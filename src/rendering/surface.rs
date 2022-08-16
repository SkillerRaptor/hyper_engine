/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::core::window::Window;
use crate::rendering::error::Error;
use crate::rendering::instance::Instance;

use ash::vk;
use log::debug;

pub struct Surface {
    pub surface: vk::SurfaceKHR,
    pub surface_loader: ash::extensions::khr::Surface,
}

impl Surface {
    pub fn new(
        window: &Window,
        entry: &ash::Entry,
        instance: &std::rc::Rc<Instance>,
    ) -> Result<Self, Error> {
        let surface_loader = ash::extensions::khr::Surface::new(entry, &instance.instance);
        let surface = unsafe {
            ash_window::create_surface(entry, &instance.instance, &window.native_window, None)?
        };

        debug!("Created vulkan surface");
        Ok(Self {
            surface,
            surface_loader,
        })
    }
}

impl Drop for Surface {
    fn drop(&mut self) {
        unsafe {
            self.surface_loader.destroy_surface(self.surface, None);
        }
    }
}
