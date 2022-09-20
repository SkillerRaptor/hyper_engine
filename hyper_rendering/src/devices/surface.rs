/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;

use ash::{
    extensions::khr::Surface as SurfaceLoader,
    vk::{self, Handle, SurfaceKHR},
    Entry, Instance,
};
use log::debug;
use thiserror::Error;
use tracing::instrument;

#[derive(Debug, Error)]
pub enum SurfaceCreationError {
    #[error("Failed to create vulkan surface")]
    SurfaceCreation(#[from] vk::Result),
}

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
    pub fn new(create_info: &SurfaceCreateInfo) -> Result<Self, SurfaceCreationError> {
        let surface_loader = SurfaceLoader::new(create_info.entry, create_info.instance);

        let (surface, result) = create_info
            .window
            .create_window_surface(create_info.instance.handle().as_raw() as usize);

        let result = vk::Result::from_raw(result as i32);
        result.result()?;

        debug!("Created vulkan surface");

        Ok(Self {
            surface: SurfaceKHR::from_raw(surface),
            surface_loader,
        })
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
