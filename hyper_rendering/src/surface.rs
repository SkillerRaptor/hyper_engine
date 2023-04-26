/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::instance::Instance;

use hyper_platform::window::Window;

use ash::{
    extensions::khr,
    vk::{self, PhysicalDevice, SurfaceKHR},
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
        let surface_loader = instance.create_surface_loader(entry);

        let surface = instance
            .create_surface(window, entry)
            .map_err(|error| CreationError::Creation(error, "surface"))?;

        Ok(Self {
            handle: surface,
            surface_loader,
        })
    }

    /// Checks if the physical device supports a surface presentation queue
    ///
    /// This function is a wrapper to not expose the internal handle
    ///
    /// Arguments:
    ///
    /// * `physical_device`: Current physical device
    /// * `queue_family_index`: Queue index
    pub fn get_physical_device_surface_support(
        &self,
        physical_device: &PhysicalDevice,
        queue_family_index: u32,
    ) -> Result<bool, vk::Result> {
        unsafe {
            self.surface_loader.get_physical_device_surface_support(
                *physical_device,
                queue_family_index,
                self.handle,
            )
        }
    }
}

impl Drop for Surface {
    fn drop(&mut self) {
        unsafe {
            self.surface_loader.destroy_surface(self.handle, None);
        }
    }
}
