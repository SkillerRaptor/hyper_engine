/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::instance::InstanceShared;

use hyper_platform::window::Window;

use ash::{extensions::khr::Surface as SurfaceFunctor, vk::SurfaceKHR};
use color_eyre::Result;

pub(crate) struct Surface {
    raw: SurfaceKHR,
    functor: SurfaceFunctor,
}

impl Surface {
    pub(crate) fn new(window: &Window, instance: &InstanceShared) -> Result<Self> {
        let functor = SurfaceFunctor::new(instance.entry(), instance.raw());
        let raw = unsafe {
            ash_window::create_surface(
                instance.entry(),
                instance.raw(),
                window.display_handle(),
                window.window_handle(),
                None,
            )
        }?;

        Ok(Self { functor, raw })
    }

    pub(crate) fn functor(&self) -> &SurfaceFunctor {
        &self.functor
    }

    pub(crate) fn raw(&self) -> SurfaceKHR {
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
