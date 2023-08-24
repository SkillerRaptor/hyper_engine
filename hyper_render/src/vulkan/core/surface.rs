/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::instance::Instance;

use hyper_platform::window::Window;

use ash::{extensions::khr, vk};
use color_eyre::Result;
use raw_window_handle::{HasRawDisplayHandle, HasRawWindowHandle};

pub(crate) struct Surface {
    raw: vk::SurfaceKHR,
    functor: khr::Surface,
}

impl Surface {
    pub(crate) fn new(create_info: SurfaceCreateInfo) -> Result<Self> {
        let SurfaceCreateInfo { window, instance } = create_info;

        let functor = khr::Surface::new(instance.entry(), instance.raw());

        let raw = unsafe {
            ash_window::create_surface(
                instance.entry(),
                instance.raw(),
                window.raw().raw_display_handle(),
                window.raw().raw_window_handle(),
                None,
            )
        }?;

        Ok(Self { raw, functor })
    }

    pub(crate) fn get_physical_device_surface_support(
        &self,
        physical_device: vk::PhysicalDevice,
        queue_family_index: u32,
    ) -> Result<bool> {
        let result = unsafe {
            self.functor.get_physical_device_surface_support(
                physical_device,
                queue_family_index,
                self.raw,
            )
        }?;

        Ok(result)
    }

    pub(crate) fn get_physical_device_surface_capabilities(
        &self,
        physical_device: vk::PhysicalDevice,
    ) -> Result<vk::SurfaceCapabilitiesKHR> {
        let result = unsafe {
            self.functor
                .get_physical_device_surface_capabilities(physical_device, self.raw)
        }?;

        Ok(result)
    }

    pub(crate) fn get_physical_device_surface_formats(
        &self,
        physical_device: vk::PhysicalDevice,
    ) -> Result<Vec<vk::SurfaceFormatKHR>> {
        let result = unsafe {
            self.functor
                .get_physical_device_surface_formats(physical_device, self.raw)
        }?;

        Ok(result)
    }

    pub(crate) fn get_physical_device_surface_present_modes(
        &self,
        physical_device: vk::PhysicalDevice,
    ) -> Result<Vec<vk::PresentModeKHR>> {
        let result = unsafe {
            self.functor
                .get_physical_device_surface_present_modes(physical_device, self.raw)
        }?;

        Ok(result)
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

pub(crate) struct SurfaceCreateInfo<'a> {
    pub(crate) window: &'a Window,
    pub(crate) instance: &'a Instance,
}
