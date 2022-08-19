/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::error::Error;
use super::instance::Instance;

use ash::extensions::khr::Surface as SurfaceLoader;
use ash::vk;
use log::debug;
use winit::window;

pub struct Surface {
    surface_loader: SurfaceLoader,
    surface: vk::SurfaceKHR,
}

impl Surface {
    pub fn new(
        window: &window::Window,
        entry: &ash::Entry,
        instance: &Instance,
    ) -> Result<Self, Error> {
        let surface_loader = SurfaceLoader::new(&entry, &instance.instance());
        let surface =
            unsafe { ash_window::create_surface(&entry, &instance.instance(), &window, None)? };

        debug!("Created vulkan surface");
        Ok(Self {
            surface,
            surface_loader,
        })
    }

    pub fn cleanup(&mut self) {
        unsafe {
            self.surface_loader.destroy_surface(self.surface, None);
        }
    }

    pub fn surface_loader(&self) -> &SurfaceLoader {
        &self.surface_loader
    }

    pub fn surface(&self) -> &vk::SurfaceKHR {
        &self.surface
    }
}
