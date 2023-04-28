/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::instance::Instance;

use hyper_platform::window::Window;

use ash::{
    extensions::khr,
    vk::{self, SurfaceKHR},
    Entry,
};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),
}

pub(crate) struct Surface {
    handle: SurfaceKHR,
    surface_loader: khr::Surface,
}

impl Surface {
    pub(crate) fn new(
        window: &Window,
        entry: &Entry,
        instance: &Instance,
    ) -> Result<Self, CreationError> {
        let surface_loader = khr::Surface::new(entry, instance.handle());

        let surface = window
            .create_surface(entry, instance.handle())
            .map_err(|error| CreationError::Creation(error, "surface"))?;

        Ok(Self {
            handle: surface,
            surface_loader,
        })
    }

    pub(crate) fn handle(&self) -> &SurfaceKHR {
        &self.handle
    }

    pub(crate) fn loader(&self) -> &khr::Surface {
        &self.surface_loader
    }
}

impl Drop for Surface {
    fn drop(&mut self) {
        unsafe {
            self.surface_loader.destroy_surface(self.handle, None);
        }
    }
}
