/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use ash::{
    khr::{surface, swapchain},
    vk,
};
use raw_window_handle::{DisplayHandle, HasDisplayHandle, HasWindowHandle, WindowHandle};

use crate::{surface::SurfaceDescriptor, vulkan::GraphicsDevice};

pub struct Surface {
    image_views: Vec<vk::ImageView>,
    images: Vec<vk::Image>,
    swapchain: vk::SwapchainKHR,
    swapchain_loader: swapchain::Device,
    inner: vk::SurfaceKHR,
    loader: surface::Instance,

    graphics_device: GraphicsDevice,
}

impl Surface {
    pub(crate) fn new(graphics_device: &GraphicsDevice, descriptor: &SurfaceDescriptor) -> Self {
        let loader = surface::Instance::new(graphics_device.entry(), graphics_device.instance());
        let surface = Self::create_surface(
            &graphics_device,
            &descriptor
                .window
                .display_handle()
                .expect("failed to get display handle"),
            &descriptor
                .window
                .window_handle()
                .expect("failed to get window handle"),
        );

        let size = descriptor.window.inner_size();
        let swapchain_loader =
            swapchain::Device::new(graphics_device.instance(), graphics_device.device());
        let (swapchain, images, image_views) = Self::create_swapchain(
            &graphics_device,
            size.x,
            size.y,
            &loader,
            surface,
            &swapchain_loader,
            vk::SwapchainKHR::null(),
        );

        Self {
            image_views,
            images,
            swapchain,
            swapchain_loader,
            inner: surface,
            loader,

            graphics_device: graphics_device.clone(),
        }
    }

    fn create_surface(
        graphics_device: &GraphicsDevice,
        display_handle: &DisplayHandle<'_>,
        window_handle: &WindowHandle<'_>,
    ) -> vk::SurfaceKHR {
        let surface = unsafe {
            ash_window::create_surface(
                graphics_device.entry(),
                graphics_device.instance(),
                display_handle.as_raw(),
                window_handle.as_raw(),
                None,
            )
        }
        .expect("failed to create surface");

        surface
    }

    fn create_swapchain(
        graphics_device: &GraphicsDevice,
        width: u32,
        height: u32,
        surface_loader: &surface::Instance,
        surface: vk::SurfaceKHR,
        loader: &swapchain::Device,
        old_swapchain: vk::SwapchainKHR,
    ) -> (vk::SwapchainKHR, Vec<vk::Image>, Vec<vk::ImageView>) {
        let surface_capabilities = unsafe {
            surface_loader.get_physical_device_surface_capabilities(
                graphics_device.physical_device(),
                surface,
            )
        }
        .expect("failed to get surface capabilities");
        let surface_extent = Self::choose_extent(width, height, &surface_capabilities);

        let surface_formats = unsafe {
            surface_loader
                .get_physical_device_surface_formats(graphics_device.physical_device(), surface)
        }
        .expect("failed to get surface formats");
        let surface_format = Self::choose_surface_format(&surface_formats);

        let surface_present_modes = unsafe {
            surface_loader.get_physical_device_surface_present_modes(
                graphics_device.physical_device(),
                surface,
            )
        }
        .expect("failed to get surface present modes");
        let surface_present_mode = Self::choose_present_mode(&surface_present_modes);

        let max_image_count = surface_capabilities.max_image_count;
        let mut image_count = surface_capabilities.min_image_count + 1;
        if max_image_count > 0 && image_count > max_image_count {
            image_count = max_image_count;
        }

        let create_info = vk::SwapchainCreateInfoKHR::default()
            .surface(surface)
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
            .old_swapchain(old_swapchain);

        let swapchain = unsafe { loader.create_swapchain(&create_info, None) }
            .expect("failed to create swapchain");

        let images = unsafe { loader.get_swapchain_images(swapchain) }
            .expect("failed to get swapchain images");
        let image_views = images
            .iter()
            .map(|&image| {
                let component_mapping = vk::ComponentMapping::default()
                    .r(vk::ComponentSwizzle::IDENTITY)
                    .g(vk::ComponentSwizzle::IDENTITY)
                    .b(vk::ComponentSwizzle::IDENTITY)
                    .a(vk::ComponentSwizzle::IDENTITY);

                let subresource_range = vk::ImageSubresourceRange::default()
                    .aspect_mask(vk::ImageAspectFlags::COLOR)
                    .base_mip_level(0)
                    .level_count(1)
                    .base_array_layer(0)
                    .layer_count(1);

                let create_info = vk::ImageViewCreateInfo::default()
                    .image(image)
                    .view_type(vk::ImageViewType::TYPE_2D)
                    .format(surface_format.format)
                    .components(component_mapping)
                    .subresource_range(subresource_range);

                let view = unsafe {
                    graphics_device
                        .device()
                        .create_image_view(&create_info, None)
                }
                .expect("failed to create image view");
                view
            })
            .collect::<Vec<_>>();

        (swapchain, images, image_views)
    }

    fn choose_extent(
        width: u32,
        height: u32,
        capabilities: &vk::SurfaceCapabilitiesKHR,
    ) -> vk::Extent2D {
        if capabilities.current_extent.width != u32::MAX
            || capabilities.current_extent.height != u32::MAX
        {
            return capabilities.current_extent;
        }

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

    fn destroy_swapchain(&self) {
        unsafe {
            self.image_views.iter().for_each(|&image_view| {
                self.graphics_device
                    .device()
                    .destroy_image_view(image_view, None);
            });

            self.swapchain_loader
                .destroy_swapchain(self.swapchain, None);
        }
    }

    pub(crate) fn resize(&mut self, width: u32, height: u32) {
        let (swapchain, images, image_views) = Self::create_swapchain(
            &self.graphics_device,
            width,
            height,
            &self.loader,
            self.inner,
            &self.swapchain_loader,
            self.swapchain,
        );

        self.destroy_swapchain();

        self.swapchain = swapchain;
        self.images = images;
        self.image_views = image_views;
    }

    pub(crate) fn loader(&self) -> &surface::Instance {
        &self.loader
    }

    pub(crate) fn inner(&self) -> vk::SurfaceKHR {
        self.inner
    }

    pub(crate) fn swapchain_loader(&self) -> &swapchain::Device {
        &self.swapchain_loader
    }

    pub(crate) fn swapchain(&self) -> vk::SwapchainKHR {
        self.swapchain
    }

    pub(crate) fn images(&self) -> &[vk::Image] {
        &self.images
    }

    pub(crate) fn image_views(&self) -> &[vk::ImageView] {
        &self.image_views
    }
}

impl Drop for Surface {
    fn drop(&mut self) {
        self.destroy_swapchain();

        unsafe {
            self.loader.destroy_surface(self.inner, None);
        }
    }
}
