/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::{Device, SwapchainSupport};
use super::super::devices::instance::Instance;
use super::super::devices::surface::Surface;
use super::super::error::Error;

use crate::core::window::Window;

use ash::extensions::khr::Swapchain as SwapchainLoader;
use ash::vk;
use log::debug;
use std::rc::Rc;
use winit::window;

pub struct Swapchain {
    format: vk::Format,
    extent: vk::Extent2D,
    image_views: Vec<vk::ImageView>,
    images: Vec<vk::Image>,
    swapchain: vk::SwapchainKHR,
    swapchain_loader: SwapchainLoader,

    device: Rc<Device>,
}

impl Swapchain {
    pub fn new(
        window: &Window,
        instance: &Rc<Instance>,
        surface: &Rc<Surface>,
        device: &Rc<Device>,
    ) -> Result<Self, Error> {
        let swapchain_loader = SwapchainLoader::new(&instance.instance(), &device.logical_device());
        let (swapchain, extent, format) = Self::create_swapchain(
            &window.native_window,
            &surface,
            &device,
            &swapchain_loader,
            false,
        )?;

        let images = unsafe { swapchain_loader.get_swapchain_images(swapchain)? };
        let image_views = Self::create_image_views(&device, &format, &images)?;

        debug!("Created vulkan swapchain");
        Ok(Self {
            format,
            extent,
            image_views,
            images,
            swapchain,
            swapchain_loader,

            device: device.clone(),
        })
    }

    fn create_swapchain(
        window: &window::Window,
        surface: &Rc<Surface>,
        device: &Rc<Device>,
        swapchain_loader: &SwapchainLoader,
        recreate: bool,
    ) -> Result<(vk::SwapchainKHR, vk::Extent2D, vk::Format), Error> {
        let support = SwapchainSupport::new(&surface, device.physical_device())?;
        let extent = Self::choose_extent(&window, support.capabilities());
        let surface_format = Self::choose_surface_format(&support.formats());
        let present_mode = Self::choose_present_mode(&support.present_modes());

        let mut image_count = support.capabilities().min_image_count + 1;
        if support.capabilities().max_image_count != 0
            && image_count > support.capabilities().max_image_count
        {
            image_count = support.capabilities().max_image_count;
        }

        // NOTE: Using exlusive sharing mode cause graphics & present queue are the same
        let image_sharing_mode = vk::SharingMode::EXCLUSIVE;

        let swapchain_create_info = vk::SwapchainCreateInfoKHR::builder()
            .surface(*surface.surface())
            .min_image_count(image_count)
            .image_format(surface_format.format)
            .image_color_space(surface_format.color_space)
            .image_extent(extent)
            .image_array_layers(1)
            .image_usage(vk::ImageUsageFlags::COLOR_ATTACHMENT)
            .image_sharing_mode(image_sharing_mode)
            .queue_family_indices(&[])
            .pre_transform(support.capabilities().current_transform)
            .composite_alpha(vk::CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(present_mode)
            .clipped(true)
            .old_swapchain(vk::SwapchainKHR::null());

        let swapchain = unsafe { swapchain_loader.create_swapchain(&swapchain_create_info, None)? };

        if !recreate {
            debug!("Swapchain Info:");
            debug!(
                "  Min Image Count: {}",
                swapchain_create_info.min_image_count
            );
            debug!(
                "  Max Image Count: {}",
                support.capabilities().max_image_count
            );
            debug!("  Image Format: {:?}", swapchain_create_info.image_format);
            debug!(
                "  Image Color Space: {:?}",
                swapchain_create_info.image_color_space
            );
            debug!(
                "  Image Extent: width={}, height={}",
                swapchain_create_info.image_extent.width, swapchain_create_info.image_extent.height
            );
            debug!(
                "  Image Array Layers: {}",
                swapchain_create_info.image_array_layers
            );
            debug!("  Image Usage: {:?}", swapchain_create_info.image_usage);
            debug!(
                "  Image Sharing Mode: {:?}",
                swapchain_create_info.image_sharing_mode
            );
            debug!("  Pre Transform: {:?}", swapchain_create_info.pre_transform);
            debug!(
                "  Composite Alpha: {:?}",
                swapchain_create_info.composite_alpha
            );
            debug!("  Present Mode: {:?}", swapchain_create_info.present_mode);
            debug!("  Clipped: {}", swapchain_create_info.clipped != 0);
        }

        Ok((swapchain, extent, surface_format.format))
    }

    fn choose_extent(
        window: &window::Window,
        capabilities: &vk::SurfaceCapabilitiesKHR,
    ) -> vk::Extent2D {
        if capabilities.current_extent.width != u32::MAX
            || capabilities.current_extent.height != u32::MAX
        {
            return capabilities.current_extent;
        }

        let size = window.inner_size();
        let clamp = |min: u32, max: u32, value: u32| min.max(max.min(value));
        let extent = vk::Extent2D {
            width: clamp(
                capabilities.min_image_extent.width,
                capabilities.max_image_extent.width,
                size.width,
            ),
            height: clamp(
                capabilities.min_image_extent.height,
                capabilities.max_image_extent.height,
                size.height,
            ),
        };

        extent
    }

    fn choose_surface_format(formats: &[vk::SurfaceFormatKHR]) -> vk::SurfaceFormatKHR {
        formats
            .iter()
            .cloned()
            .find(|surface_format| {
                surface_format.format == vk::Format::B8G8R8A8_SRGB
                    && surface_format.color_space == vk::ColorSpaceKHR::SRGB_NONLINEAR
            })
            .unwrap_or_else(|| formats[0])
    }

    fn choose_present_mode(present_modes: &[vk::PresentModeKHR]) -> vk::PresentModeKHR {
        present_modes
            .iter()
            .cloned()
            .find(|present_mode| *present_mode == vk::PresentModeKHR::MAILBOX)
            .unwrap_or_else(|| vk::PresentModeKHR::FIFO)
    }

    fn create_image_views(
        device: &Device,
        format: &vk::Format,
        images: &Vec<vk::Image>,
    ) -> Result<Vec<vk::ImageView>, Error> {
        let image_views = images
            .iter()
            .map(|image| {
                let components = vk::ComponentMapping::builder()
                    .r(vk::ComponentSwizzle::IDENTITY)
                    .g(vk::ComponentSwizzle::IDENTITY)
                    .b(vk::ComponentSwizzle::IDENTITY)
                    .a(vk::ComponentSwizzle::IDENTITY);

                let subsource_range = vk::ImageSubresourceRange::builder()
                    .aspect_mask(vk::ImageAspectFlags::COLOR)
                    .base_mip_level(0)
                    .level_count(1)
                    .base_array_layer(0)
                    .layer_count(1);

                let image_view_create_info = vk::ImageViewCreateInfo::builder()
                    .image(*image)
                    .view_type(vk::ImageViewType::TYPE_2D)
                    .format(*format)
                    .components(*components)
                    .subresource_range(*subsource_range);

                unsafe {
                    device
                        .logical_device()
                        .create_image_view(&image_view_create_info, None)
                }
            })
            .collect::<Result<Vec<_>, _>>()?;

        Ok(image_views)
    }

    pub fn recreate(
        &mut self,
        window: &window::Window,
        surface: &Rc<Surface>,
        device: &Rc<Device>,
    ) -> Result<(), Error> {
        unsafe {
            device.logical_device().device_wait_idle()?;
        }

        self.cleanup();

        let (swapchain, extent, format) =
            Self::create_swapchain(window, &surface, &device, &self.swapchain_loader, true)?;

        self.swapchain = swapchain;
        self.extent = extent;
        self.format = format;

        let images = unsafe { self.swapchain_loader.get_swapchain_images(swapchain)? };
        let image_views = Self::create_image_views(&device, &format, &images)?;

        self.images = images;
        self.image_views = image_views;

        Ok(())
    }

    pub fn cleanup(&mut self) {
        unsafe {
            self.image_views.iter().for_each(|image_view| {
                self.device
                    .logical_device()
                    .destroy_image_view(*image_view, None)
            });
            self.swapchain_loader
                .destroy_swapchain(self.swapchain, None);
        }
    }

    pub fn swapchain_loader(&self) -> &SwapchainLoader {
        &self.swapchain_loader
    }

    pub fn swapchain(&self) -> &vk::SwapchainKHR {
        &self.swapchain
    }

    pub fn images(&self) -> &Vec<vk::Image> {
        &self.images
    }

    pub fn image_views(&self) -> &Vec<vk::ImageView> {
        &self.image_views
    }

    pub fn extent(&self) -> &vk::Extent2D {
        &self.extent
    }

    pub fn format(&self) -> &vk::Format {
        &self.format
    }
}

impl Drop for Swapchain {
    fn drop(&mut self) {
        self.cleanup();
    }
}
