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

/// An enum representing the errors that can occur while constructing a surface
#[derive(Debug, Error)]
pub enum CreationError {
    /// Creation of a vulkan object failed
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),
}

pub(crate) struct Surface {
    /// Ash vulkan surface handle
    handle: SurfaceKHR,

    /// Ash surface loader
    surface_loader: khr::Surface,
}

impl Surface {
    /// Constructs a new surface
    ///
    /// Arguments:
    ///
    /// * `window`: Application window
    /// * `entry`: Ash vulkan entry loader
    /// * `instance`: Vulkan Instance
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

    /// Returns the vulkan surface handle
    pub(crate) fn handle(&self) -> &SurfaceKHR {
        &self.handle
    }

    /// Returns the vulkan surface loader
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
