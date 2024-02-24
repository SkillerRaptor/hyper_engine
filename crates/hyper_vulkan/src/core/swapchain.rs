/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::sync::Arc;

use ash::{extensions::khr, vk};
use color_eyre::eyre::Result;
use hyper_platform::window::Window;

use crate::{
    core::{device::DeviceShared, surface::Surface},
    resource::image::Image,
    sync::binary_semaphore::BinarySemaphore,
};

pub struct Swapchain {
    width: u32,
    height: u32,
    images: Vec<Image>,
    raw: vk::SwapchainKHR,
    functor: khr::Swapchain,
    device: Arc<DeviceShared>,
}

impl Swapchain {
    pub(crate) fn new(
        window: &Window,
        surface: &Surface,
        device: &Arc<DeviceShared>,
    ) -> Result<Self> {
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

        let swapchain_images = unsafe { functor.get_swapchain_images(raw) }?;

        let images = swapchain_images
            .iter()
            .map(|&swapchain_image| {
                Image::new_external(
                    device,
                    swapchain_image,
                    surface_extent,
                    surface_format.format,
                )
            })
            .collect::<Result<Vec<_>, _>>()?;

        Ok(Self {
            width: window.framebuffer_size().0,
            height: window.framebuffer_size().1,
            images,
            raw,
            functor,
            device: Arc::clone(device),
        })
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

    pub fn acquire_next_image(&self, present_semaphore: &BinarySemaphore) -> Result<Option<u32>> {
        match unsafe {
            self.functor.acquire_next_image(
                self.raw,
                1_000_000_000,
                present_semaphore.raw(),
                vk::Fence::null(),
            )
        } {
            Ok((index, _)) => Ok(Some(index)),
            Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => Ok(None),
            Err(error) => Err(error.into()),
        }
    }

    pub fn present(
        &self,
        render_semaphore: &BinarySemaphore,
        swapchain_image_index: u32,
    ) -> Result<()> {
        let swapchains = &[self.raw];
        let wait_semaphores = &[render_semaphore.raw()];
        let image_indices = &[swapchain_image_index];

        let present_info = vk::PresentInfoKHR::builder()
            .swapchains(swapchains)
            .wait_semaphores(wait_semaphores)
            .image_indices(image_indices);

        unsafe {
            let recreate = match self
                .functor
                .queue_present(self.device.queue(), &present_info)
            {
                Ok(recreate) => recreate,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => true,
                Err(error) => return Err(error.into()),
            };

            if recreate {
                todo!("Recreate Swapchain")
            }
        }

        Ok(())
    }

    pub fn images(&self) -> &[Image] {
        &self.images
    }

    pub fn width(&self) -> u32 {
        self.width
    }

    pub fn height(&self) -> u32 {
        self.height
    }
}

impl Drop for Swapchain {
    fn drop(&mut self) {
        unsafe {
            self.functor.destroy_swapchain(self.raw, None);
        }
    }
}
