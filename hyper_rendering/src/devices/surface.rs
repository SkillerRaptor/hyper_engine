/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;

use ash::{extensions::khr::Surface as SurfaceLoader, vk::SurfaceKHR, Entry, Instance};
use log::debug;
use tracing::instrument;

pub(crate) struct SurfaceCreateInfo<'a> {
    pub window: &'a Window,
    pub entry: &'a Entry,
    pub instance: &'a Instance,
}

pub(crate) struct Surface {
    surface: SurfaceKHR,
    surface_loader: SurfaceLoader,
}

impl Surface {
    #[instrument(skip_all)]
    pub fn new(create_info: &SurfaceCreateInfo) -> Self {
        let surface_loader = SurfaceLoader::new(create_info.entry, create_info.instance);

        let surface = create_info
            .window
            .create_window_surface(create_info.instance);

        debug!("Created vulkan surface");

        Self {
            surface,
            surface_loader,
        }
    }

    pub fn surface_loader(&self) -> &SurfaceLoader {
        &self.surface_loader
    }

    pub fn surface(&self) -> &SurfaceKHR {
        &self.surface
    }
}

impl Drop for Surface {
    #[instrument(skip_all)]
    fn drop(&mut self) {
        unsafe {
            self.surface_loader.destroy_surface(self.surface, None);
        }
    }
}
