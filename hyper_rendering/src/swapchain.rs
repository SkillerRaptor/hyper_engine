/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    binary_semaphore::BinarySemaphore,
    device::{
        queue_family_indices::QueueFamilyIndices,
        swapchain_support_details::SwapchainSupportDetails, Device,
    },
    error::{CreationError, CreationResult, RuntimeError, RuntimeResult},
    instance::Instance,
    surface::Surface,
};

use hyper_platform::window::Window;

use ash::{extensions::khr, vk};
use std::sync::Arc;

pub(crate) struct Swapchain {
    image_views: Vec<vk::ImageView>,
    images: Vec<vk::Image>,

    extent: vk::Extent2D,
    format: vk::Format,

    handle: vk::SwapchainKHR,
    loader: khr::Swapchain,

    device: Arc<Device>,
}

impl Swapchain {
    pub(crate) fn new(
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        device: Arc<Device>,
    ) -> CreationResult<Self> {
        let swapchain_support_details =
            SwapchainSupportDetails::new(surface, device.physical_device())?;

        let capabilities = swapchain_support_details.capabilities();

        let extent = Self::choose_extent(window, capabilities);
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

        let (image_sharing_mode, queue_family_indices_array): (vk::SharingMode, &[u32]) =
            if queue_families[0] != queue_families[1] {
                (vk::SharingMode::CONCURRENT, &queue_families)
            } else {
                (vk::SharingMode::EXCLUSIVE, &[])
            };

        let create_info = vk::SwapchainCreateInfoKHR::builder()
            .surface(surface.handle())
            .min_image_count(image_count)
            .image_format(surface_format.format)
            .image_color_space(surface_format.color_space)
            .image_extent(extent)
            .image_array_layers(1)
            .image_usage(vk::ImageUsageFlags::COLOR_ATTACHMENT)
            .image_sharing_mode(image_sharing_mode)
            .queue_family_indices(queue_family_indices_array)
            .pre_transform(capabilities.current_transform)
            .composite_alpha(vk::CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(present_mode)
            .clipped(true)
            .old_swapchain(vk::SwapchainKHR::null());

        let loader = khr::Swapchain::new(instance.handle(), device.handle());

        let handle = unsafe {
            loader
                .create_swapchain(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "swapchain"))
        }?;

        let images = unsafe {
            loader
                .get_swapchain_images(handle)
                .map_err(|error| CreationError::VulkanAllocation(error, "swapchain images"))
        }?;

        let mut image_views = Vec::new();
        for image in &images {
            let component_mapping = vk::ComponentMapping::builder()
                .r(vk::ComponentSwizzle::IDENTITY)
                .g(vk::ComponentSwizzle::IDENTITY)
                .b(vk::ComponentSwizzle::IDENTITY)
                .a(vk::ComponentSwizzle::IDENTITY);

            let subresource_range = vk::ImageSubresourceRange::builder()
                .aspect_mask(vk::ImageAspectFlags::COLOR)
                .base_mip_level(0)
                .level_count(1)
                .base_array_layer(0)
                .layer_count(1);

            let create_info = vk::ImageViewCreateInfo::builder()
                .image(*image)
                .view_type(vk::ImageViewType::TYPE_2D)
                .format(surface_format.format)
                .components(*component_mapping)
                .subresource_range(*subresource_range);

            let handle = unsafe {
                device
                    .handle()
                    .create_image_view(&create_info, None)
                    .map_err(|error| CreationError::VulkanCreation(error, "image view"))
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

    fn choose_extent(window: &Window, capabilities: vk::SurfaceCapabilitiesKHR) -> vk::Extent2D {
        if capabilities.current_extent.width != u32::MAX
            || capabilities.current_extent.height != u32::MAX
        {
            return capabilities.current_extent;
        }

        let framebuffer_size = window.framebuffer_size();

        vk::Extent2D {
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

    pub(crate) fn acquire_next_image(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        present_semaphore: &BinarySemaphore,
    ) -> RuntimeResult<Option<u32>> {
        unsafe {
            match self.loader.acquire_next_image(
                self.handle,
                1_000_000_000,
                present_semaphore.handle(),
                vk::Fence::null(),
            ) {
                Ok((index, _)) => Ok(Some(index)),
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    self.recreate(window, instance, surface)?;
                    Ok(None)
                }
                Err(error) => Err(RuntimeError::ImageAcquisition(error)),
            }
        }
    }

    pub(crate) fn present_queue(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        queue: vk::Queue,
        render_semaphore: &BinarySemaphore,
        swapchain_image_index: u32,
    ) -> RuntimeResult<()> {
        let swapchains = &[self.handle];
        let wait_semaphores = &[render_semaphore.handle()];
        let image_indices = &[swapchain_image_index];

        let present_info = vk::PresentInfoKHR::builder()
            .swapchains(swapchains)
            .wait_semaphores(wait_semaphores)
            .image_indices(image_indices);

        unsafe {
            let recreate = match self.loader.queue_present(queue, &present_info) {
                Ok(recreate) => recreate,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => true,
                Err(_) => panic!("unhandled error"),
            };

            if recreate {
                self.recreate(window, instance, surface)?;
            }
        }

        Ok(())
    }

    pub(crate) fn recreate(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
    ) -> RuntimeResult<()> {
        if window.framebuffer_size() == (0, 0) {
            return Ok(());
        }

        self.device.wait_idle()?;

        self.destroy();

        let swapchain_support_details =
            SwapchainSupportDetails::new(surface, self.device.physical_device())
                .map_err(|error| RuntimeError::CreationError(error, "swapchain support details"))?;

        let capabilities = swapchain_support_details.capabilities();

        let extent = Self::choose_extent(window, capabilities);
        let surface_format = Self::choose_surface_format(swapchain_support_details.formats());
        let present_mode = Self::choose_present_mode(swapchain_support_details.present_modes());

        let mut image_count = capabilities.min_image_count + 1;
        if capabilities.max_image_count > 0 && image_count > capabilities.max_image_count {
            image_count = capabilities.max_image_count;
        }

        let queue_family_indices =
            QueueFamilyIndices::new(instance, surface, self.device.physical_device())
                .map_err(|error| RuntimeError::CreationError(error, "queue family indices"))?;

        let queue_families = [
            queue_family_indices.graphics_family().unwrap(),
            queue_family_indices.present_family().unwrap(),
        ];

        let (image_sharing_mode, queue_family_indices_array): (vk::SharingMode, &[u32]) =
            if queue_families[0] != queue_families[1] {
                (vk::SharingMode::CONCURRENT, &queue_families)
            } else {
                (vk::SharingMode::EXCLUSIVE, &[])
            };

        let create_info = vk::SwapchainCreateInfoKHR::builder()
            .surface(surface.handle())
            .min_image_count(image_count)
            .image_format(surface_format.format)
            .image_color_space(surface_format.color_space)
            .image_extent(extent)
            .image_array_layers(1)
            .image_usage(vk::ImageUsageFlags::COLOR_ATTACHMENT)
            .image_sharing_mode(image_sharing_mode)
            .queue_family_indices(queue_family_indices_array)
            .pre_transform(capabilities.current_transform)
            .composite_alpha(vk::CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(present_mode)
            .clipped(true)
            .old_swapchain(vk::SwapchainKHR::null());

        self.handle = unsafe {
            self.loader
                .create_swapchain(&create_info, None)
                .map_err(|error| {
                    RuntimeError::CreationError(
                        CreationError::VulkanCreation(error, "swapchain"),
                        "swapchain",
                    )
                })
        }?;

        self.images = unsafe {
            self.loader
                .get_swapchain_images(self.handle)
                .map_err(|error| {
                    RuntimeError::CreationError(
                        CreationError::VulkanAllocation(error, "swapchain images"),
                        "swapchain images",
                    )
                })
        }?;

        self.image_views = Vec::new();
        for image in &self.images {
            let component_mapping = vk::ComponentMapping::builder()
                .r(vk::ComponentSwizzle::IDENTITY)
                .g(vk::ComponentSwizzle::IDENTITY)
                .b(vk::ComponentSwizzle::IDENTITY)
                .a(vk::ComponentSwizzle::IDENTITY);

            let subresource_range = vk::ImageSubresourceRange::builder()
                .aspect_mask(vk::ImageAspectFlags::COLOR)
                .base_mip_level(0)
                .level_count(1)
                .base_array_layer(0)
                .layer_count(1);

            let create_info = vk::ImageViewCreateInfo::builder()
                .image(*image)
                .view_type(vk::ImageViewType::TYPE_2D)
                .format(surface_format.format)
                .components(*component_mapping)
                .subresource_range(*subresource_range);

            let handle = unsafe {
                self.device
                    .handle()
                    .create_image_view(&create_info, None)
                    .map_err(|error| {
                        RuntimeError::CreationError(
                            CreationError::VulkanCreation(error, "image view"),
                            "image view",
                        )
                    })
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

    pub(crate) fn format(&self) -> vk::Format {
        self.format
    }

    pub(crate) fn extent(&self) -> vk::Extent2D {
        self.extent
    }

    pub(crate) fn images(&self) -> &[vk::Image] {
        &self.images
    }

    pub(crate) fn image_views(&self) -> &[vk::ImageView] {
        &self.image_views
    }
}

impl Drop for Swapchain {
    fn drop(&mut self) {
        self.destroy();
    }
}
