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
    #[error("failed to create vulkan {1}")]
    Creation(#[source] vk::Result, &'static str),
}

pub(crate) struct Surface {
    handle: SurfaceKHR,
    loader: khr::Surface,
}

impl Surface {
    pub(crate) fn new(
        window: &Window,
        entry: &Entry,
        instance: &Instance,
    ) -> Result<Self, CreationError> {
        let loader = khr::Surface::new(entry, instance.handle());

        let handle = window
            .create_surface(entry, instance.handle())
            .map_err(|error| CreationError::Creation(error, "surface"))?;

        Ok(Self { handle, loader })
    }

    pub(crate) fn handle(&self) -> &SurfaceKHR {
        &self.handle
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
