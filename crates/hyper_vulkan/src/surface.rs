/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::instance::{InstanceExtensions, InstanceShared};

use ash::{
    extensions::khr::{self, Surface as SurfaceFunctor},
    vk::SurfaceKHR,
};
use color_eyre::{eyre::eyre, Result};
use raw_window_handle::{HasRawDisplayHandle, HasRawWindowHandle};
use std::{ffi::CStr, str};

pub struct Surface {
    raw: SurfaceKHR,
    functor: SurfaceFunctor,
}

impl Surface {
    pub(crate) fn new<W>(instance: &InstanceShared, window: &W) -> Result<Self>
    where
        W: HasRawDisplayHandle + HasRawWindowHandle,
    {
        let functor = SurfaceFunctor::new(instance.entry().raw(), instance.raw());
        let raw = unsafe {
            ash_window::create_surface(
                instance.entry().raw(),
                instance.raw(),
                window.raw_display_handle(),
                window.raw_window_handle(),
                None,
            )
        }?;

        Ok(Self { raw, functor })
    }

    pub fn required_extensions<W>(display_handle: &W) -> Result<InstanceExtensions>
    where
        W: HasRawDisplayHandle,
    {
        let raw_extensions =
            ash_window::enumerate_required_extensions(display_handle.raw_display_handle())?;
        let extensions = raw_extensions
            .iter()
            .map(|&ptr| unsafe { CStr::from_ptr(ptr) }.to_str())
            .collect::<Result<Vec<_>, _>>()?;

        // NOTE: This is hacky, but there is no other way to get a &str from a CStr in const
        const KHR_SURFACE: &str =
            unsafe { str::from_utf8_unchecked(khr::Surface::name().to_bytes()) };
        const KHR_WIN32_SURFACE: &str =
            unsafe { str::from_utf8_unchecked(khr::Win32Surface::name().to_bytes()) };

        let mut instance_extensions = InstanceExtensions::default();
        for extension in extensions {
            match extension {
                KHR_SURFACE => instance_extensions.khr_surface = true,
                KHR_WIN32_SURFACE => instance_extensions.khr_win32_surface = true,
                _ => return Err(eyre!("Found unknown instance extension: {}", extension)),
            }
        }

        Ok(instance_extensions)
    }

    pub(super) fn functor(&self) -> &SurfaceFunctor {
        &self.functor
    }

    pub(super) fn raw(&self) -> SurfaceKHR {
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
