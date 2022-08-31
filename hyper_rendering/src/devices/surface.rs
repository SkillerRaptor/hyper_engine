/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::devices::instance::Instance;

use hyper_platform::window::Window;

use ash::{extensions::khr::Surface as SurfaceLoader, vk::SurfaceKHR, Entry};
use log::debug;

pub(crate) struct Surface {
    surface_loader: SurfaceLoader,
    surface: SurfaceKHR,
}

impl Surface {
    pub fn new(window: &Window, entry: &Entry, instance: &Instance) -> Self {
        let surface_loader = SurfaceLoader::new(entry, instance.instance());
        let surface = window.create_window_surface(instance.instance());

        debug!("Created vulkan surface");
        Self {
            surface,
            surface_loader,
        }
    }

    pub fn cleanup(&mut self) {
        unsafe {
            self.surface_loader.destroy_surface(self.surface, None);
        }
    }

    pub fn surface_loader(&self) -> &SurfaceLoader {
        &self.surface_loader
    }

    pub fn surface(&self) -> &SurfaceKHR {
        &self.surface
    }
}
