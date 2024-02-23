/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{extensions::khr, vk};
use color_eyre::eyre::Result;
use hyper_platform::window::Window;
use raw_window_handle::{HasRawDisplayHandle, HasRawWindowHandle};

use crate::instance::InstanceShared;

pub struct Surface {
    raw: vk::SurfaceKHR,
    functor: khr::Surface,
}

impl Surface {
    pub(crate) fn new(window: &Window, instance: &InstanceShared) -> Result<Self> {
        let functor = khr::Surface::new(instance.entry(), instance.raw());
        let surface = unsafe {
            ash_window::create_surface(
                instance.entry(),
                instance.raw(),
                window.raw_display_handle(),
                window.raw_window_handle(),
                None,
            )
        }?;

        Ok(Self {
            raw: surface,
            functor,
        })
    }

    pub(crate) fn functor(&self) -> &khr::Surface {
        &self.functor
    }

    pub(crate) fn raw(&self) -> vk::SurfaceKHR {
        self.raw
    }
}

impl Drop for Surface {
    fn drop(&mut self) {
        unsafe {
            self.functor.destroy_surface(self.raw, None);
        }
    }
}
