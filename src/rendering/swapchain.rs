/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::core::window::Window;
use crate::rendering::device::Device;
use crate::rendering::error::Error;
use crate::rendering::instance::Instance;
use crate::rendering::surface::Surface;

use ash::vk;
use log::debug;

pub struct SwapchainSupport {
    pub capabilities: vk::SurfaceCapabilitiesKHR,
    pub formats: Vec<vk::SurfaceFormatKHR>,
    pub present_modes: Vec<vk::PresentModeKHR>,
}

impl SwapchainSupport {
    pub fn new(
        surface_loader: &ash::extensions::khr::Surface,
        surface: vk::SurfaceKHR,
        physical_device: vk::PhysicalDevice,
    ) -> Result<Self, Error> {
        let capabilities = unsafe {
            surface_loader.get_physical_device_surface_capabilities(physical_device, surface)?
        };

        let formats = unsafe {
            surface_loader.get_physical_device_surface_formats(physical_device, surface)?
        };

        let present_modes = unsafe {
            surface_loader.get_physical_device_surface_present_modes(physical_device, surface)?
        };

        Ok(Self {
            capabilities,
            formats,
            present_modes,
        })
    }
}

pub struct Swapchain {
    pub format: vk::Format,
    pub extent: vk::Extent2D,
    pub image_views: Vec<vk::ImageView>,
    pub images: Vec<vk::Image>,
    pub swapchain: vk::SwapchainKHR,
    pub swapchain_loader: ash::extensions::khr::Swapchain,
}

impl Swapchain {
    pub fn new(
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        device: &Device,
    ) -> Result<Self, Error> {
        let swapchain_loader =
            ash::extensions::khr::Swapchain::new(&instance.instance, &device.device);
        let (swapchain, extent, format) = Self::create_swapchain(
            &window.native_window,
            &surface.surface_loader,
            surface.surface,
            device.physical_device,
            &swapchain_loader,
            false,
        )?;

        let images = unsafe { swapchain_loader.get_swapchain_images(swapchain)? };
        let image_views = Self::create_image_views(&device.device, format, &images)?;

        debug!("Created vulkan swapchain");
        Ok(Self {
            format,
            extent,
            image_views,
            images,
            swapchain,
            swapchain_loader,
        })
    }

    fn create_swapchain(
        window: &winit::window::Window,
        surface_loader: &ash::extensions::khr::Surface,
        surface: vk::SurfaceKHR,
        physical_device: vk::PhysicalDevice,
        swapchain_loader: &ash::extensions::khr::Swapchain,
        recreate: bool,
    ) -> Result<(vk::SwapchainKHR, vk::Extent2D, vk::Format), Error> {
        let support = SwapchainSupport::new(&surface_loader, surface, physical_device)?;
        let extent = Self::choose_extent(&window, support.capabilities);
        let surface_format = Self::choose_surface_format(&support.formats);
        let present_mode = Self::choose_present_mode(&support.present_modes);

        let mut image_count = support.capabilities.min_image_count + 1;
        if support.capabilities.max_image_count != 0
            && image_count > support.capabilities.max_image_count
        {
            image_count = support.capabilities.max_image_count;
        }

        // NOTE: Using exlusive sharing mode cause graphics & present queue are the same
        let image_sharing_mode = vk::SharingMode::EXCLUSIVE;

        let swapchain_create_info = vk::SwapchainCreateInfoKHR {
            surface: surface,
            min_image_count: image_count,
            image_format: surface_format.format,
            image_color_space: surface_format.color_space,
            image_extent: extent,
            image_array_layers: 1,
            image_usage: vk::ImageUsageFlags::COLOR_ATTACHMENT,
            image_sharing_mode: image_sharing_mode,
            queue_family_index_count: 0,
            p_queue_family_indices: std::ptr::null(),
            pre_transform: support.capabilities.current_transform,
            composite_alpha: vk::CompositeAlphaFlagsKHR::OPAQUE,
            present_mode: present_mode,
            clipped: true as u32,
            old_swapchain: vk::SwapchainKHR::null(),
            ..Default::default()
        };

        let swapchain = unsafe { swapchain_loader.create_swapchain(&swapchain_create_info, None)? };

        if !recreate {
            debug!("Swapchain Info:");
            debug!(
                "  Min Image Count: {}",
                swapchain_create_info.min_image_count
            );
            debug!(
                "  Max Image Count: {}",
                support.capabilities.max_image_count
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
        window: &winit::window::Window,
        capabilities: vk::SurfaceCapabilitiesKHR,
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
        device: &ash::Device,
        format: vk::Format,
        images: &Vec<vk::Image>,
    ) -> Result<Vec<vk::ImageView>, Error> {
        let image_views = images
            .iter()
            .map(|image| {
                let components = vk::ComponentMapping {
                    r: vk::ComponentSwizzle::IDENTITY,
                    g: vk::ComponentSwizzle::IDENTITY,
                    b: vk::ComponentSwizzle::IDENTITY,
                    a: vk::ComponentSwizzle::IDENTITY,
                    ..Default::default()
                };

                let subsource_range = vk::ImageSubresourceRange {
                    aspect_mask: vk::ImageAspectFlags::COLOR,
                    base_mip_level: 0,
                    level_count: 1,
                    base_array_layer: 0,
                    layer_count: 1,
                    ..Default::default()
                };

                let image_view_create_info = vk::ImageViewCreateInfo {
                    image: *image,
                    view_type: vk::ImageViewType::TYPE_2D,
                    format: format,
                    components: components,
                    subresource_range: subsource_range,
                    ..Default::default()
                };

                unsafe { device.create_image_view(&image_view_create_info, None) }
            })
            .collect::<Result<Vec<_>, _>>()?;

        Ok(image_views)
    }

    pub fn recreate(
        &mut self,
        window: &winit::window::Window,
        surface: vk::SurfaceKHR,
        surface_loader: &ash::extensions::khr::Surface,
        physical_device: vk::PhysicalDevice,
        device: &ash::Device,
    ) -> Result<(), Error> {
        unsafe {
            device.device_wait_idle()?;
        }

        self.cleanup(device);

        let (swapchain, extent, format) = Self::create_swapchain(
            &window,
            &surface_loader,
            surface,
            physical_device,
            &self.swapchain_loader,
            true,
        )?;

        self.swapchain = swapchain;
        self.extent = extent;
        self.format = format;

        let images = unsafe { self.swapchain_loader.get_swapchain_images(swapchain)? };
        let image_views = Self::create_image_views(&device, format, &images)?;

        self.images = images;
        self.image_views = image_views;

        Ok(())
    }

    pub fn cleanup(&mut self, device: &ash::Device) {
        unsafe {
            self.image_views
                .iter()
                .for_each(|image_view| device.destroy_image_view(*image_view, None));
            self.swapchain_loader
                .destroy_swapchain(self.swapchain, None);
        }
    }
}
