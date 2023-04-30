/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::{
        queue_family_indices::{self, QueueFamilyIndices},
        swapchain_support_details::{self, SwapchainSupportDetails},
        Device,
    },
    instance::Instance,
    surface::Surface,
    sync::binary_semaphore::BinarySemaphore,
};

use hyper_platform::window::Window;

use ash::{
    extensions::khr,
    vk::{
        self, ColorSpaceKHR, ComponentMapping, ComponentSwizzle, CompositeAlphaFlagsKHR, Extent2D,
        Format, Image, ImageAspectFlags, ImageSubresourceRange, ImageUsageFlags, ImageView,
        ImageViewCreateInfo, ImageViewType, PresentInfoKHR, PresentModeKHR, Queue, SharingMode,
        SurfaceCapabilitiesKHR, SurfaceFormatKHR, SwapchainCreateInfoKHR, SwapchainKHR,
    },
};
use core::panic;
use std::sync::Arc;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("failed to create vulkan {1}")]
    Creation(#[source] vk::Result, &'static str),

    #[error("failed to find memory from device")]
    OutOfMemory(#[source] vk::Result),

    #[error("failed to create queue family indices")]
    QueueFamilyIndicesFailure(#[from] queue_family_indices::CreationError),

    #[error("failed to create swapchain support details")]
    SwapchainSupportDetailsFailure(#[from] swapchain_support_details::CreationError),
}

pub(crate) struct Swapchain {
    image_views: Vec<ImageView>,
    images: Vec<Image>,

    extent: Extent2D,
    format: Format,

    handle: SwapchainKHR,
    loader: khr::Swapchain,

    device: Arc<Device>,
}

impl Swapchain {
    pub(crate) fn new(
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        device: Arc<Device>,
    ) -> Result<Self, CreationError> {
        let swapchain_support_details =
            SwapchainSupportDetails::new(surface, device.physical_device())?;

        let capabilities = swapchain_support_details.capabilities();

        let extent = Self::choose_extent(window, &capabilities);
        let surface_format = Self::choose_surface_format(swapchain_support_details.formats());
        let present_mode = Self::choose_present_mode(swapchain_support_details.present_modes());

        let mut image_count = capabilities.min_image_count + 1;
        if capabilities.max_image_count > 0 && image_count > capabilities.max_image_count {
            image_count = capabilities.max_image_count;
        }

        let queue_family_indices =
            QueueFamilyIndices::new(instance, surface, device.physical_device())?;
        let queue_families = [
            queue_family_indices.graphics_family().unwrap(),
            queue_family_indices.present_family().unwrap(),
        ];

        let (image_sharing_mode, queue_family_indices_array): (SharingMode, &[u32]) =
            if queue_families[0] != queue_families[1] {
                (SharingMode::CONCURRENT, &queue_families)
            } else {
                (SharingMode::EXCLUSIVE, &[])
            };

        let create_info = SwapchainCreateInfoKHR::builder()
            .surface(*surface.handle())
            .min_image_count(image_count)
            .image_format(surface_format.format)
            .image_color_space(surface_format.color_space)
            .image_extent(extent)
            .image_array_layers(1)
            .image_usage(ImageUsageFlags::COLOR_ATTACHMENT)
            .image_sharing_mode(image_sharing_mode)
            .queue_family_indices(queue_family_indices_array)
            .pre_transform(capabilities.current_transform)
            .composite_alpha(CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(present_mode)
            .clipped(true)
            .old_swapchain(SwapchainKHR::null());

        let loader = khr::Swapchain::new(instance.handle(), device.handle());

        let handle = unsafe {
            loader
                .create_swapchain(&create_info, None)
                .map_err(|error| CreationError::Creation(error, "swapchain"))
        }?;

        let images = unsafe {
            loader
                .get_swapchain_images(handle)
                .map_err(CreationError::OutOfMemory)
        }?;

        let mut image_views = Vec::new();
        for image in &images {
            let component_mapping = ComponentMapping::builder()
                .r(ComponentSwizzle::IDENTITY)
                .g(ComponentSwizzle::IDENTITY)
                .b(ComponentSwizzle::IDENTITY)
                .a(ComponentSwizzle::IDENTITY);

            let subresource_range = ImageSubresourceRange::builder()
                .aspect_mask(ImageAspectFlags::COLOR)
                .base_mip_level(0)
                .level_count(1)
                .base_array_layer(0)
                .layer_count(1);

            let create_info = ImageViewCreateInfo::builder()
                .image(*image)
                .view_type(ImageViewType::TYPE_2D)
                .format(surface_format.format)
                .components(*component_mapping)
                .subresource_range(*subresource_range);

            let handle = unsafe {
                device
                    .handle()
                    .create_image_view(&create_info, None)
                    .map_err(|error| CreationError::Creation(error, "image view"))
            }?;

            image_views.push(handle);
        }

        Ok(Self {
            image_views,
            images,

            extent,
            format: surface_format.format,

            handle,
            loader,

            device,
        })
    }

    fn choose_extent(window: &Window, capabilities: &SurfaceCapabilitiesKHR) -> Extent2D {
        if capabilities.current_extent.width != u32::MAX
            || capabilities.current_extent.height != u32::MAX
        {
            return capabilities.current_extent;
        }

        let framebuffer_size = window.framebuffer_size();

        Extent2D {
            width: framebuffer_size.0.clamp(
                capabilities.min_image_extent.width,
                capabilities.max_image_extent.width,
            ),
            height: framebuffer_size.1.clamp(
                capabilities.min_image_extent.height,
                capabilities.max_image_extent.height,
            ),
        }
    }

    fn choose_surface_format(formats: &[SurfaceFormatKHR]) -> SurfaceFormatKHR {
        *formats
            .iter()
            .find(|&format| {
                format.format == Format::B8G8R8A8_SRGB
                    && format.color_space == ColorSpaceKHR::SRGB_NONLINEAR
            })
            .unwrap_or_else(|| &formats[0])
    }

    fn choose_present_mode(present_modes: &[PresentModeKHR]) -> PresentModeKHR {
        *present_modes
            .iter()
            .find(|&present_mode| *present_mode == PresentModeKHR::MAILBOX)
            .unwrap_or(&PresentModeKHR::FIFO)
    }

    pub(crate) fn acquire_next_image(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        present_semaphore: &BinarySemaphore,
    ) -> Option<u32> {
        // TODO: Propagate error

        unsafe {
            match self.loader.acquire_next_image(
                self.handle,
                1_000_000_000,
                *present_semaphore.handle(),
                vk::Fence::null(),
            ) {
                Ok((index, _)) => Some(index),
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    self.recreate(window, instance, surface).unwrap();
                    None
                }
                Err(_) => panic!("unhandled error"),
            }
        }
    }

    pub(crate) fn present_queue(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        queue: &Queue,
        render_semaphore: &BinarySemaphore,
        swapchain_image_index: u32,
    ) {
        // TODO: Propagte error

        let swapchains = &[self.handle];
        let wait_semaphores = &[*render_semaphore.handle()];
        let image_indices = &[swapchain_image_index];

        let present_info = PresentInfoKHR::builder()
            .swapchains(swapchains)
            .wait_semaphores(wait_semaphores)
            .image_indices(image_indices);

        unsafe {
            let recreate = match self.loader.queue_present(*queue, &present_info) {
                Ok(recreate) => recreate,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => true,
                Err(_) => panic!("unhandled error"),
            };

            if recreate {
                self.recreate(window, instance, surface).unwrap();
            }
        }
    }

    pub(crate) fn recreate(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
    ) -> Result<(), CreationError> {
        if window.framebuffer_size() == (0, 0) {
            return Ok(());
        }

        self.device.wait_idle();

        self.destroy();

        let swapchain_support_details =
            SwapchainSupportDetails::new(surface, self.device.physical_device())?;

        let capabilities = swapchain_support_details.capabilities();

        let extent = Self::choose_extent(window, &capabilities);
        let surface_format = Self::choose_surface_format(swapchain_support_details.formats());
        let present_mode = Self::choose_present_mode(swapchain_support_details.present_modes());

        let mut image_count = capabilities.min_image_count + 1;
        if capabilities.max_image_count > 0 && image_count > capabilities.max_image_count {
            image_count = capabilities.max_image_count;
        }

        let queue_family_indices =
            QueueFamilyIndices::new(instance, surface, self.device.physical_device())?;
        let queue_families = [
            queue_family_indices.graphics_family().unwrap(),
            queue_family_indices.present_family().unwrap(),
        ];

        let (image_sharing_mode, queue_family_indices_array): (SharingMode, &[u32]) =
            if queue_families[0] != queue_families[1] {
                (SharingMode::CONCURRENT, &queue_families)
            } else {
                (SharingMode::EXCLUSIVE, &[])
            };

        let create_info = SwapchainCreateInfoKHR::builder()
            .surface(*surface.handle())
            .min_image_count(image_count)
            .image_format(surface_format.format)
            .image_color_space(surface_format.color_space)
            .image_extent(extent)
            .image_array_layers(1)
            .image_usage(ImageUsageFlags::COLOR_ATTACHMENT)
            .image_sharing_mode(image_sharing_mode)
            .queue_family_indices(queue_family_indices_array)
            .pre_transform(capabilities.current_transform)
            .composite_alpha(CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(present_mode)
            .clipped(true)
            .old_swapchain(SwapchainKHR::null());

        self.handle = unsafe {
            self.loader
                .create_swapchain(&create_info, None)
                .map_err(|error| CreationError::Creation(error, "swapchain"))
        }?;

        self.images = unsafe {
            self.loader
                .get_swapchain_images(self.handle)
                .map_err(CreationError::OutOfMemory)
        }?;

        self.image_views = Vec::new();
        for image in &self.images {
            let component_mapping = ComponentMapping::builder()
                .r(ComponentSwizzle::IDENTITY)
                .g(ComponentSwizzle::IDENTITY)
                .b(ComponentSwizzle::IDENTITY)
                .a(ComponentSwizzle::IDENTITY);

            let subresource_range = ImageSubresourceRange::builder()
                .aspect_mask(ImageAspectFlags::COLOR)
                .base_mip_level(0)
                .level_count(1)
                .base_array_layer(0)
                .layer_count(1);

            let create_info = ImageViewCreateInfo::builder()
                .image(*image)
                .view_type(ImageViewType::TYPE_2D)
                .format(surface_format.format)
                .components(*component_mapping)
                .subresource_range(*subresource_range);

            let handle = unsafe {
                self.device
                    .handle()
                    .create_image_view(&create_info, None)
                    .map_err(|error| CreationError::Creation(error, "image view"))
            }?;

            self.image_views.push(handle);
        }

        self.extent = extent;
        self.format = surface_format.format;

        Ok(())
    }

    fn destroy(&mut self) {
        unsafe {
            for image_view in &self.image_views {
                self.device.handle().destroy_image_view(*image_view, None);
            }

            self.loader.destroy_swapchain(self.handle, None);
        }
    }

    pub(crate) fn format(&self) -> &Format {
        &self.format
    }

    pub(crate) fn extent(&self) -> &Extent2D {
        &self.extent
    }

    pub(crate) fn images(&self) -> &[Image] {
        &self.images
    }

    pub(crate) fn image_views(&self) -> &[ImageView] {
        &self.image_views
    }
}

impl Drop for Swapchain {
    fn drop(&mut self) {
        self.destroy();
    }
}
