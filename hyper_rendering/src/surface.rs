/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    error::{Error, Result},
    instance::Instance,
};

use hyper_platform::window::Window;

use ash::{extensions::khr, vk, Entry};

pub(crate) struct Surface {
    handle: vk::SurfaceKHR,
    loader: khr::Surface,
}

impl Surface {
    pub(crate) fn new(window: &Window, entry: &Entry, instance: &Instance) -> Result<Self> {
        let loader = khr::Surface::new(entry, instance.handle());

        let handle = window
            .create_surface(entry, instance.handle())
            .map_err(|error| Error::VulkanCreation(error, "surface"))?;

        Ok(Self { handle, loader })
    }

    pub(crate) fn handle(&self) -> vk::SurfaceKHR {
        self.handle
    }

    pub(crate) fn loader(&self) -> &khr::Surface {
        &self.loader
    }
}

impl Drop for Surface {
    fn drop(&mut self) {
        unsafe {
            self.loader.destroy_surface(self.handle, None);
        }
    }
}
