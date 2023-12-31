/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{device::DeviceShared, surface::Surface};

use hyper_platform::window::Window;

use ash::{extensions::khr, vk};
use color_eyre::eyre::Result;

pub struct Swapchain {
    raw: vk::SwapchainKHR,
    functor: khr::Swapchain,
}

impl Swapchain {
    pub(crate) fn new(window: &Window, surface: &Surface, device: &DeviceShared) -> Result<Self> {
        let functor = khr::Swapchain::new(device.instance().raw(), device.raw());

        let surface_capabilities = unsafe {
            surface
                .functor()
                .get_physical_device_surface_capabilities(device.physical_device(), surface.raw())
        }?;
        let surface_extent = Self::choose_extent(window, &surface_capabilities);

        let surface_formats = unsafe {
            surface
                .functor()
                .get_physical_device_surface_formats(device.physical_device(), surface.raw())
        }?;
        let surface_format = Self::choose_surface_format(&surface_formats);

        let surface_present_modes = unsafe {
            surface
                .functor()
                .get_physical_device_surface_present_modes(device.physical_device(), surface.raw())
        }?;
        let surface_present_mode = Self::choose_present_mode(&surface_present_modes);

        let max_image_count = surface_capabilities.max_image_count;
        let mut image_count = surface_capabilities.min_image_count + 1;
        if max_image_count > 0 && image_count > max_image_count {
            image_count = max_image_count;
        }

        // TODO: Take old swapchain
        let create_info = vk::SwapchainCreateInfoKHR::builder()
            .surface(surface.raw())
            .min_image_count(image_count)
            .image_format(surface_format.format)
            .image_color_space(surface_format.color_space)
            .image_extent(surface_extent)
            .image_array_layers(1)
            .image_usage(vk::ImageUsageFlags::COLOR_ATTACHMENT | vk::ImageUsageFlags::TRANSFER_DST)
            .image_sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[])
            .pre_transform(surface_capabilities.current_transform)
            .composite_alpha(vk::CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(surface_present_mode)
            .clipped(true)
            .old_swapchain(vk::SwapchainKHR::null());

        let raw = unsafe { functor.create_swapchain(&create_info, None) }?;

        Ok(Self { raw, functor })
    }

    fn choose_extent(window: &Window, capabilities: &vk::SurfaceCapabilitiesKHR) -> vk::Extent2D {
        if capabilities.current_extent.width != u32::MAX
            || capabilities.current_extent.height != u32::MAX
        {
            return capabilities.current_extent;
        }

        let (width, height) = window.framebuffer_size();

        vk::Extent2D {
            width: width.clamp(
                capabilities.min_image_extent.width,
                capabilities.max_image_extent.width,
            ),
            height: height.clamp(
                capabilities.min_image_extent.height,
                capabilities.max_image_extent.height,
            ),
        }
    }

    fn choose_surface_format(formats: &[vk::SurfaceFormatKHR]) -> vk::SurfaceFormatKHR {
        *formats
            .iter()
            .find(|&format| {
                format.format == vk::Format::B8G8R8A8_SRGB
                    && format.color_space == vk::ColorSpaceKHR::SRGB_NONLINEAR
            })
            .unwrap_or_else(|| &formats[0])
    }

    fn choose_present_mode(present_modes: &[vk::PresentModeKHR]) -> vk::PresentModeKHR {
        *present_modes
            .iter()
            .find(|&present_mode| *present_mode == vk::PresentModeKHR::MAILBOX)
            .unwrap_or(&vk::PresentModeKHR::FIFO)
    }
}

impl Drop for Swapchain {
    fn drop(&mut self) {
        unsafe {
            self.functor.destroy_swapchain(self.raw, None);
        }
    }
}
