/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    core::{
        device::{
            queue_family_indices::QueueFamilyIndices,
            swapchain_support_details::SwapchainSupportDetails, Device,
        },
        instance::Instance,
        surface::Surface,
    },
    memory::allocator::{
        Allocation, AllocationCreateInfo, AllocationScheme, Allocator, MemoryLocation,
    },
    sync::binary_semaphore::BinarySemaphore,
};

use hyper_platform::window::Window;

use ash::{extensions::khr, vk};
use color_eyre::Result;
use std::{cell::RefCell, rc::Rc};

pub(crate) struct Swapchain {
    // TODO: Abstract into image struct
    depth_image_allocation: Option<Allocation>,
    depth_format: vk::Format,
    depth_image_view: vk::ImageView,
    depth_image: vk::Image,

    image_views: Vec<vk::ImageView>,
    images: Vec<vk::Image>,

    extent: vk::Extent2D,
    format: vk::Format,

    handle: vk::SwapchainKHR,
    loader: khr::Swapchain,

    allocator: Rc<RefCell<Allocator>>,
    device: Rc<Device>,
}

impl Swapchain {
    pub(crate) fn new(
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
    ) -> Result<Self> {
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
            .surface(surface.raw())
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

        let loader = khr::Swapchain::new(instance.raw(), device.handle());

        let handle = unsafe { loader.create_swapchain(&create_info, None) }?;

        let images = unsafe { loader.get_swapchain_images(handle) }?;

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

            let handle = unsafe { device.handle().create_image_view(&create_info, None) }?;

            image_views.push(handle);
        }

        let (depth_image, depth_image_view, depth_format, depth_image_allocation) =
            Self::create_depth_image(window, &device, &allocator)?;

        Ok(Self {
            depth_image_allocation: Some(depth_image_allocation),
            depth_format,
            depth_image_view,
            depth_image,

            image_views,
            images,

            extent,
            format: surface_format.format,

            handle,
            loader,

            allocator,
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

    fn create_depth_image(
        window: &Window,
        device: &Device,
        allocator: &RefCell<Allocator>,
    ) -> Result<(vk::Image, vk::ImageView, vk::Format, Allocation)> {
        let framebuffer_size = window.framebuffer_size();

        let extent = vk::Extent3D::builder()
            .width(framebuffer_size.0)
            .height(framebuffer_size.1)
            .depth(1);

        let format = vk::Format::D32_SFLOAT;

        let image_create_info = vk::ImageCreateInfo::builder()
            .image_type(vk::ImageType::TYPE_2D)
            .format(format)
            .extent(*extent)
            .mip_levels(1)
            .array_layers(1)
            .samples(vk::SampleCountFlags::TYPE_1)
            .tiling(vk::ImageTiling::OPTIMAL)
            .usage(vk::ImageUsageFlags::DEPTH_STENCIL_ATTACHMENT)
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[])
            .initial_layout(vk::ImageLayout::UNDEFINED);

        let image = unsafe { device.handle().create_image(&image_create_info, None) }?;

        let memory_requirements = unsafe { device.handle().get_image_memory_requirements(image) };

        let allocation = allocator.borrow_mut().allocate(AllocationCreateInfo {
            label: Some("depth image"),
            requirements: memory_requirements,
            location: MemoryLocation::GpuOnly,
            scheme: AllocationScheme::DedicatedImage(image),
        })?;

        unsafe {
            device.handle().bind_image_memory(
                image,
                allocation.handle().memory(),
                allocation.handle().offset(),
            )?;
        }

        let subsource_range = vk::ImageSubresourceRange::builder()
            .aspect_mask(vk::ImageAspectFlags::DEPTH)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_view_create_info = vk::ImageViewCreateInfo::builder()
            .image(image)
            .view_type(vk::ImageViewType::TYPE_2D)
            .format(format)
            .components(vk::ComponentMapping::default())
            .subresource_range(*subsource_range);

        let image_view = unsafe {
            device
                .handle()
                .create_image_view(&image_view_create_info, None)
        }?;

        Ok((image, image_view, format, allocation))
    }

    pub(crate) fn acquire_next_image(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        present_semaphore: &BinarySemaphore,
    ) -> Result<Option<u32>> {
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
                Err(error) => Err(error.into()),
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
    ) -> Result<()> {
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
                Err(error) => return Err(error.into()),
            };

            if recreate {
                self.recreate(window, instance, surface)?;
            }
        }

        Ok(())
    }

    // TODO: Improve this by abstracting into functions and avoid repition
    pub(crate) fn recreate(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
    ) -> Result<()> {
        if window.framebuffer_size() == (0, 0) {
            return Ok(());
        }

        self.device.wait_idle()?;

        self.destroy();

        let swapchain_support_details =
            SwapchainSupportDetails::new(surface, self.device.physical_device())?;

        let capabilities = swapchain_support_details.capabilities();

        let extent = Self::choose_extent(window, capabilities);
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

        let (image_sharing_mode, queue_family_indices_array): (vk::SharingMode, &[u32]) =
            if queue_families[0] != queue_families[1] {
                (vk::SharingMode::CONCURRENT, &queue_families)
            } else {
                (vk::SharingMode::EXCLUSIVE, &[])
            };

        let create_info = vk::SwapchainCreateInfoKHR::builder()
            .surface(surface.raw())
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

        self.handle = unsafe { self.loader.create_swapchain(&create_info, None) }?;

        self.images = unsafe { self.loader.get_swapchain_images(self.handle) }?;

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

            let handle = unsafe { self.device.handle().create_image_view(&create_info, None) }?;

            self.image_views.push(handle);
        }

        self.extent = extent;
        self.format = surface_format.format;

        let (depth_image, depth_image_view, depth_format, depth_image_allocation) =
            Self::create_depth_image(window, &self.device, &self.allocator)?;

        self.depth_image = depth_image;
        self.depth_image_view = depth_image_view;
        self.depth_format = depth_format;
        self.depth_image_allocation = Some(depth_image_allocation);

        Ok(())
    }

    fn destroy(&mut self) {
        unsafe {
            self.device
                .handle()
                .destroy_image_view(self.depth_image_view, None);
            self.allocator
                .borrow_mut()
                .free(self.depth_image_allocation.take().unwrap())
                .unwrap();

            self.device.handle().destroy_image(self.depth_image, None);

            for image_view in &self.image_views {
                self.device.handle().destroy_image_view(*image_view, None);
            }

            self.loader.destroy_swapchain(self.handle, None);
        }
    }

    pub(crate) fn depth_image_format(&self) -> vk::Format {
        self.depth_format
    }

    pub(crate) fn depth_image_view(&self) -> vk::ImageView {
        self.depth_image_view
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
