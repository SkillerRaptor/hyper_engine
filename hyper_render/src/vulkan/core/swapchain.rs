/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    core::{device::Device, instance::Instance, surface::Surface},
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

    raw: vk::SwapchainKHR,
    functor: khr::Swapchain,

    allocator: Rc<RefCell<Allocator>>,
    device: Rc<Device>,
}

impl Swapchain {
    pub(crate) fn new(create_info: SwapchainCreateInfo) -> Result<Self> {
        let SwapchainCreateInfo {
            window,
            instance,
            surface,
            device,
            allocator,
        } = create_info;

        let settings = Self::choose_settings(window, &device);

        let functor = khr::Swapchain::new(instance.raw(), device.logical_device().raw());
        let raw = Self::create_swapchain(window, surface, &device, &settings, &functor)?;

        let (images, image_views) =
            Self::create_images_and_views(&device, &functor, raw, &settings)?;
        let (depth_image, depth_image_view, depth_format, depth_image_allocation) =
            Self::create_depth_image(window, &device, &allocator)?;

        Ok(Self {
            depth_image_allocation: Some(depth_image_allocation),
            depth_format,
            depth_image_view,
            depth_image,

            image_views,
            images,

            extent: settings.extent,
            format: settings.surface_format.format,

            raw,
            functor,

            allocator,
            device,
        })
    }

    fn choose_settings(window: &Window, device: &Device) -> SwapchainSettings {
        let surface_details = device.surface_details();

        let capabilities = surface_details.capabilities();

        let extent = Self::choose_extent(window, capabilities);
        let surface_format = Self::choose_surface_format(surface_details.formats());
        let present_mode = Self::choose_present_mode(surface_details.present_modes());

        SwapchainSettings {
            extent,
            surface_format,
            present_mode,
        }
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

    fn create_swapchain(
        window: &Window,
        surface: &Surface,
        device: &Device,
        settings: &SwapchainSettings,
        functor: &khr::Swapchain,
    ) -> Result<vk::SwapchainKHR> {
        let surface_details = device.surface_details();
        let capabilities = surface_details.capabilities();

        let mut image_count = capabilities.min_image_count + 1;
        if capabilities.max_image_count > 0 && image_count > capabilities.max_image_count {
            image_count = capabilities.max_image_count;
        }

        let queue_families = device.queue_families();
        let queue_families = [
            queue_families.graphics_family(),
            queue_families.present_family(),
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
            .image_format(settings.surface_format.format)
            .image_color_space(settings.surface_format.color_space)
            .image_extent(settings.extent)
            .image_array_layers(1)
            .image_usage(vk::ImageUsageFlags::COLOR_ATTACHMENT)
            .image_sharing_mode(image_sharing_mode)
            .queue_family_indices(queue_family_indices_array)
            .pre_transform(capabilities.current_transform)
            .composite_alpha(vk::CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(settings.present_mode)
            .clipped(true)
            .old_swapchain(vk::SwapchainKHR::null());

        let raw = unsafe { functor.create_swapchain(&create_info, None) }?;
        Ok(raw)
    }

    fn create_images_and_views(
        device: &Device,
        functor: &khr::Swapchain,
        raw: vk::SwapchainKHR,
        settings: &SwapchainSettings,
    ) -> Result<(Vec<vk::Image>, Vec<vk::ImageView>)> {
        let images = unsafe { functor.get_swapchain_images(raw) }?;

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
                .format(settings.surface_format.format)
                .components(*component_mapping)
                .subresource_range(*subresource_range);

            let raw = device.create_vk_image_view(*create_info)?;
            image_views.push(raw);
        }

        Ok((images, image_views))
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

        let image = device.create_vk_image(*image_create_info)?;

        let memory_requirements = device.get_image_memory_requirements(image);

        let allocation = allocator.borrow_mut().allocate(AllocationCreateInfo {
            label: Some("depth image"),
            requirements: memory_requirements,
            location: MemoryLocation::GpuOnly,
            scheme: AllocationScheme::DedicatedImage(image),
        })?;

        device.bind_image_memory(image, allocation.memory(), allocation.offset())?;

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

        let image_view = device.create_vk_image_view(*image_view_create_info)?;

        Ok((image, image_view, format, allocation))
    }

    pub(crate) fn acquire_next_image(
        &mut self,
        window: &Window,
        surface: &Surface,
        present_semaphore: &BinarySemaphore,
    ) -> Result<Option<u32>> {
        unsafe {
            match self.functor.acquire_next_image(
                self.raw,
                1_000_000_000,
                present_semaphore.raw(),
                vk::Fence::null(),
            ) {
                Ok((index, _)) => Ok(Some(index)),
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    self.recreate(window, surface)?;
                    Ok(None)
                }
                Err(error) => Err(error.into()),
            }
        }
    }

    pub(crate) fn present_queue(
        &mut self,
        window: &Window,
        surface: &Surface,
        queue: vk::Queue,
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
            let recreate = match self.functor.queue_present(queue, &present_info) {
                Ok(recreate) => recreate,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => true,
                Err(error) => return Err(error.into()),
            };

            if recreate {
                self.recreate(window, surface)?;
            }
        }

        Ok(())
    }

    pub(crate) fn recreate(&mut self, window: &Window, surface: &Surface) -> Result<()> {
        if window.framebuffer_size() == (0, 0) {
            return Ok(());
        }

        self.device.wait_idle()?;

        self.destroy();

        let settings = Self::choose_settings(window, &self.device);

        let raw = Self::create_swapchain(window, surface, &self.device, &settings, &self.functor)?;

        let (images, image_views) =
            Self::create_images_and_views(&self.device, &self.functor, raw, &settings)?;
        let (depth_image, depth_image_view, depth_format, depth_image_allocation) =
            Self::create_depth_image(window, &self.device, &self.allocator)?;

        self.extent = settings.extent;
        self.format = settings.surface_format.format;

        self.raw = raw;

        self.images = images;
        self.image_views = image_views;

        self.depth_image = depth_image;
        self.depth_image_view = depth_image_view;
        self.depth_format = depth_format;
        self.depth_image_allocation = Some(depth_image_allocation);

        Ok(())
    }

    fn destroy(&mut self) {
        self.device.destroy_image_view(self.depth_image_view);
        self.allocator
            .borrow_mut()
            .free(self.depth_image_allocation.take().unwrap())
            .unwrap();

        self.device.destroy_image(self.depth_image);
        self.image_views.iter().for_each(|&image_view| {
            self.device.destroy_image_view(image_view);
        });

        unsafe {
            self.functor.destroy_swapchain(self.raw, None);
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

pub(crate) struct SwapchainCreateInfo<'a> {
    pub(crate) window: &'a Window,
    pub(crate) instance: &'a Instance,
    pub(crate) surface: &'a Surface,
    pub(crate) device: Rc<Device>,
    pub(crate) allocator: Rc<RefCell<Allocator>>,
}

pub(crate) struct SwapchainSettings {
    extent: vk::Extent2D,
    surface_format: vk::SurfaceFormatKHR,
    present_mode: vk::PresentModeKHR,
}
