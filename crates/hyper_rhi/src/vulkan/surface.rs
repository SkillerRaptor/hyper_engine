/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::sync::atomic::Ordering;

use ash::{
    khr::{surface, swapchain},
    vk,
};
use raw_window_handle::{DisplayHandle, HasDisplayHandle, HasWindowHandle, WindowHandle};

use crate::{
    surface::SurfaceDescriptor,
    vulkan::{GraphicsDevice, Texture},
};

pub struct Surface {
    height: u32,
    width: u32,
    resized: bool,

    current_texture_index: u32,
    textures: Vec<Texture>,

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
        let swapchain = Self::create_swapchain(
            &graphics_device,
            size.x,
            size.y,
            &loader,
            surface,
            &swapchain_loader,
            vk::SwapchainKHR::null(),
        );

        let textures = Self::create_textures(
            graphics_device,
            &swapchain_loader,
            swapchain,
            size.x,
            size.y,
        );

        Self {
            height: size.y,
            width: size.x,
            resized: false,

            current_texture_index: 0,

            textures,
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
    ) -> vk::SwapchainKHR {
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

        swapchain
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
                format.format == vk::Format::B8G8R8A8_UNORM
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

    fn create_textures(
        graphics_device: &GraphicsDevice,
        loader: &swapchain::Device,
        swapchain: vk::SwapchainKHR,
        width: u32,
        height: u32,
    ) -> Vec<Texture> {
        let images = unsafe { loader.get_swapchain_images(swapchain) }
            .expect("failed to get swapchain images");

        // TODO: Don't hardcode the format
        let textures = images
            .iter()
            .map(|&image| {
                Texture::new_external(
                    graphics_device,
                    image,
                    width,
                    height,
                    vk::Format::B8G8R8A8_UNORM,
                )
            })
            .collect::<Vec<_>>();

        textures
    }

    fn destroy_swapchain(&mut self) {
        unsafe {
            self.textures.clear();

            self.swapchain_loader
                .destroy_swapchain(self.swapchain, None);
        }
    }

    pub(crate) fn resize(&mut self, width: u32, height: u32) {
        self.height = height;
        self.width = width;
        self.resized = true;
    }

    pub(crate) fn current_texture(&mut self) -> Texture {
        // Resize
        if self.resized {
            let swapchain = Self::create_swapchain(
                &self.graphics_device,
                self.width,
                self.height,
                &self.loader,
                self.inner,
                &self.swapchain_loader,
                self.swapchain,
            );

            let textures = Self::create_textures(
                &self.graphics_device,
                &self.swapchain_loader,
                swapchain,
                self.width,
                self.height,
            );

            self.destroy_swapchain();

            self.swapchain = swapchain;
            self.textures = textures;

            self.resized = false;
        }

        let semaphores = [self.graphics_device.submit_semaphore()];
        let values = [self
            .graphics_device
            .current_frame_index()
            .load(Ordering::Relaxed) as u64
            - 1];
        let wait_info = vk::SemaphoreWaitInfo::default()
            .semaphores(&semaphores)
            .values(&values);

        unsafe {
            self.graphics_device
                .device()
                .wait_semaphores(&wait_info, u64::MAX)
                .expect("failed to wait for semaphore");
        }

        let (index, _) = unsafe {
            self.swapchain_loader.acquire_next_image(
                self.swapchain,
                u64::MAX,
                self.graphics_device.current_frame().present_semaphore,
                vk::Fence::null(),
            )
        }
        .expect("failed to acquire next image index");

        self.current_texture_index = index;

        self.textures[self.current_texture_index as usize].clone()
    }

    pub(crate) fn present(&mut self) {
        let swapchains = [self.swapchain];
        let wait_semaphores = [self.graphics_device.current_frame().render_semaphore];
        let image_indices = [self.current_texture_index];
        let present_info = vk::PresentInfoKHR::default()
            .swapchains(&swapchains)
            .wait_semaphores(&wait_semaphores)
            .image_indices(&image_indices);

        unsafe {
            self.swapchain_loader
                .queue_present(self.graphics_device.queue(), &present_info)
                .expect("failed to present swap chain");
        };

        let value = self
            .graphics_device
            .current_frame_index()
            .load(Ordering::Relaxed);
        self.graphics_device
            .current_frame_index()
            .store(value, Ordering::Relaxed);
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

    pub(crate) fn textures(&self) -> &[Texture] {
        &self.textures
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
