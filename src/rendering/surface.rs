/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::core::window::Window;
use crate::rendering::instance::Instance;

use ash::vk;
use log::info;

pub enum SurfaceError {
    VulkanError(vk::Result),
}

impl std::fmt::Display for SurfaceError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            SurfaceError::VulkanError(error) => {
                write!(formatter, "{}", error)
            }
        }
    }
}

impl From<vk::Result> for SurfaceError {
    fn from(error: vk::Result) -> Self {
        SurfaceError::VulkanError(error)
    }
}

pub struct Surface {
    pub surface_loader: ash::extensions::khr::Surface,
    pub surface: ash::vk::SurfaceKHR,
}

impl Surface {
    pub fn new(
        window: &Window,
        entry: &ash::Entry,
        instance: &Instance,
    ) -> Result<Self, SurfaceError> {
        let surface_loader = ash::extensions::khr::Surface::new(entry, &instance.instance);
        let surface = unsafe {
            ash_window::create_surface(entry, &instance.instance, &window.native_window, None)?
        };

        info!("Successfully created surface");
        Ok(Self {
            surface_loader,
            surface,
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
