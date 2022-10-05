/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{AllocationError, Allocator, MemoryLocation},
    devices::device::{SwapchainSupport, SwapchainSupportCreationError},
};

use hyper_platform::window::Window;

use ash::{
    extensions::khr::{Surface as SurfaceLoader, Swapchain as SwapchainLoader},
    vk::{
        self, ColorSpaceKHR, ComponentMapping, ComponentSwizzle, CompositeAlphaFlagsKHR, Extent2D,
        Extent3D, Format, Image, ImageAspectFlags, ImageCreateInfo, ImageLayout,
        ImageSubresourceRange, ImageTiling, ImageType, ImageUsageFlags, ImageView,
        ImageViewCreateInfo, ImageViewType, PhysicalDevice, PresentModeKHR, SampleCountFlags,
        SharingMode, SurfaceCapabilitiesKHR, SurfaceFormatKHR, SurfaceKHR, SwapchainCreateInfoKHR,
        SwapchainKHR,
    },
    Device, Instance,
};
use gpu_allocator::vulkan::Allocation;
use log::debug;
use std::{cell::RefCell, rc::Rc};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum SwapchainCreationError {
    #[error("Failed to allocate depth image memory")]
    Allocation(#[from] AllocationError),

    #[error("Failed to aquire swapchain images")]
    SwapchainImagesAcquisition(vk::Result),

    #[error("Failed to bind image memory")]
    ImageMemoryBinding(vk::Result),

    #[error("Failed to create depth image")]
    DepthImageCreation(vk::Result),

    #[error("Failed to create depth image view")]
    DepthImageViewCreation(vk::Result),

    #[error("Failed to create swapchain image view")]
    ImageViewCreation(vk::Result),

    #[error("Failed to create vulkan swapchain")]
    SwapchainCreation(vk::Result),

    #[error("Failed to create swapchain support")]
    SwapchainSupportCreation(#[from] SwapchainSupportCreationError),
}

pub(crate) struct SwapchainCreateInfo<'a> {
    pub window: &'a Window,
    pub instance: &'a Instance,
    pub surface_loader: &'a SurfaceLoader,
    pub surface: &'a SurfaceKHR,
    pub physical_device: &'a PhysicalDevice,
    pub logical_device: &'a Device,
    pub allocator: &'a Rc<RefCell<Allocator>>,
}
pub(crate) struct Swapchain {
    depth_image_allocation: Option<Allocation>,
    depth_format: Format,
    depth_image_view: ImageView,
    depth_image: Image,

    image_views: Vec<ImageView>,
    images: Vec<Image>,

    extent: Extent2D,
    format: Format,

    swapchain: SwapchainKHR,
    swapchain_loader: SwapchainLoader,

    allocator: Rc<RefCell<Allocator>>,
    logical_device: Device,
    physical_device: PhysicalDevice,
    surface: SurfaceKHR,
    surface_loader: SurfaceLoader,
}

impl Swapchain {
    pub fn new(create_info: &SwapchainCreateInfo) -> Result<Self, SwapchainCreationError> {
        let swapchain_loader =
            SwapchainLoader::new(create_info.instance, create_info.logical_device);

        let (swapchain, extent, format) = Self::create_swapchain(
            create_info.window,
            create_info.surface_loader,
            create_info.surface,
            create_info.physical_device,
            &swapchain_loader,
            false,
        )?;

        let images = Self::create_images(&swapchain_loader, &swapchain, false)?;

        let image_views =
            Self::create_image_views(create_info.logical_device, &format, &images, false)?;

        let (depth_image, depth_image_view, depth_format, depth_image_allocation) =
            Self::create_depth_image(
                create_info.window,
                create_info.logical_device,
                create_info.allocator,
                false,
            )?;

        Ok(Self {
            depth_image_allocation: Some(depth_image_allocation),
            depth_format,
            depth_image_view,
            depth_image,

            image_views,
            images,

            extent,
            format,

            swapchain,
            swapchain_loader,

            allocator: create_info.allocator.clone(),
            logical_device: create_info.logical_device.clone(),
            physical_device: *create_info.physical_device,
            surface: *create_info.surface,
            surface_loader: create_info.surface_loader.clone(),
        })
    }

    fn create_swapchain(
        window: &Window,
        surface_loader: &SurfaceLoader,
        surface: &SurfaceKHR,
        physical_device: &PhysicalDevice,
        swapchain_loader: &SwapchainLoader,
        recreated: bool,
    ) -> Result<(SwapchainKHR, Extent2D, Format), SwapchainCreationError> {
        let support = SwapchainSupport::new(surface_loader, surface, physical_device)?;

        let extent = Self::choose_extent(window, support.capabilities());
        let format = Self::choose_surface_format(support.formats());
        let present_mode = Self::choose_present_mode(support.present_modes());

        let image_count = {
            let count = support.capabilities().min_image_count + 1;
            if support.capabilities().max_image_count != 0
                && count > support.capabilities().max_image_count
            {
                support.capabilities().max_image_count
            } else {
                count
            }
        };

        // NOTE: Using exlusive sharing mode cause graphics & present queue are the same
        let image_sharing_mode = SharingMode::EXCLUSIVE;

        let swapchain_create_info = SwapchainCreateInfoKHR::builder()
            .surface(*surface)
            .min_image_count(image_count)
            .image_format(format.format)
            .image_color_space(format.color_space)
            .image_extent(extent)
            .image_array_layers(1)
            .image_usage(ImageUsageFlags::COLOR_ATTACHMENT)
            .image_sharing_mode(image_sharing_mode)
            .queue_family_indices(&[])
            .pre_transform(support.capabilities().current_transform)
            .composite_alpha(CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(present_mode)
            .clipped(true)
            .old_swapchain(SwapchainKHR::null());

        let swapchain = unsafe {
            swapchain_loader
                .create_swapchain(&swapchain_create_info, None)
                .map_err(SwapchainCreationError::SwapchainCreation)?
        };

        if !recreated {
            debug!("Created swapchain");

            Self::print_swapchain_information(&support, &swapchain_create_info);
        }

        Ok((swapchain, extent, format.format))
    }

    fn choose_extent(window: &Window, capabilities: &SurfaceCapabilitiesKHR) -> Extent2D {
        if capabilities.current_extent.width != u32::MAX
            || capabilities.current_extent.height != u32::MAX
        {
            return capabilities.current_extent;
        }

        let clamp = |min: u32, max: u32, value: u32| min.max(max.min(value));

        Extent2D {
            width: clamp(
                capabilities.min_image_extent.width,
                capabilities.max_image_extent.width,
                window.framebuffer_width() as u32,
            ),
            height: clamp(
                capabilities.min_image_extent.height,
                capabilities.max_image_extent.height,
                window.framebuffer_height() as u32,
            ),
        }
    }

    fn choose_surface_format(formats: &[SurfaceFormatKHR]) -> SurfaceFormatKHR {
        formats
            .iter()
            .cloned()
            .find(|format| {
                format.format == Format::B8G8R8A8_SRGB
                    && format.color_space == ColorSpaceKHR::SRGB_NONLINEAR
            })
            .unwrap_or_else(|| formats[0])
    }

    fn choose_present_mode(present_modes: &[PresentModeKHR]) -> PresentModeKHR {
        present_modes
            .iter()
            .cloned()
            .find(|present_mode| *present_mode == PresentModeKHR::MAILBOX)
            .unwrap_or(PresentModeKHR::FIFO)
    }

    fn print_swapchain_information(
        swapchain_support: &SwapchainSupport,
        swapchain_create_info: &SwapchainCreateInfoKHR,
    ) {
        debug!("Swapchain Info:");
        debug!(
            "  Min Image Count: {}",
            swapchain_create_info.min_image_count
        );
        debug!(
            "  Max Image Count: {}",
            swapchain_support.capabilities().max_image_count
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

    fn create_images(
        swapchain_loader: &SwapchainLoader,
        swapchain: &SwapchainKHR,
        recreated: bool,
    ) -> Result<Vec<Image>, SwapchainCreationError> {
        let images = unsafe {
            swapchain_loader
                .get_swapchain_images(*swapchain)
                .map_err(SwapchainCreationError::SwapchainImagesAcquisition)?
        };

        if !recreated {
            debug!("Created images");
        }

        Ok(images)
    }

    fn create_image_views(
        logical_device: &Device,
        format: &Format,
        images: &[Image],
        recreated: bool,
    ) -> Result<Vec<ImageView>, SwapchainCreationError> {
        let image_views = images
            .iter()
            .map(|image| {
                let components = ComponentMapping::builder()
                    .r(ComponentSwizzle::IDENTITY)
                    .g(ComponentSwizzle::IDENTITY)
                    .b(ComponentSwizzle::IDENTITY)
                    .a(ComponentSwizzle::IDENTITY);

                let subsource_range = ImageSubresourceRange::builder()
                    .aspect_mask(ImageAspectFlags::COLOR)
                    .base_mip_level(0)
                    .level_count(1)
                    .base_array_layer(0)
                    .layer_count(1);

                let image_view_create_info = ImageViewCreateInfo::builder()
                    .image(*image)
                    .view_type(ImageViewType::TYPE_2D)
                    .format(*format)
                    .components(*components)
                    .subresource_range(*subsource_range);

                unsafe { logical_device.create_image_view(&image_view_create_info, None) }
            })
            .collect::<Result<Vec<_>, _>>()
            .map_err(SwapchainCreationError::ImageViewCreation)?;

        if !recreated {
            debug!("Created image views");
        }

        Ok(image_views)
    }

    fn create_depth_image(
        window: &Window,
        logical_device: &Device,
        allocator: &Rc<RefCell<Allocator>>,
        recreated: bool,
    ) -> Result<(Image, ImageView, Format, Allocation), SwapchainCreationError> {
        let extent = Extent3D::builder()
            .width(window.framebuffer_width() as u32)
            .height(window.framebuffer_height() as u32)
            .depth(1);

        let format = Format::D32_SFLOAT;

        // NOTE: Hardcoding the depth format to 32 bit float
        let image_create_info = ImageCreateInfo::builder()
            .image_type(ImageType::TYPE_2D)
            .format(format)
            .extent(*extent)
            .mip_levels(1)
            .array_layers(1)
            .samples(SampleCountFlags::TYPE_1)
            .tiling(ImageTiling::OPTIMAL)
            .usage(ImageUsageFlags::DEPTH_STENCIL_ATTACHMENT)
            .sharing_mode(SharingMode::EXCLUSIVE)
            .queue_family_indices(&[])
            .initial_layout(ImageLayout::UNDEFINED);

        let image = unsafe {
            logical_device
                .create_image(&image_create_info, None)
                .map_err(SwapchainCreationError::DepthImageCreation)?
        };

        if !recreated {
            debug!("Created depth image");
        }

        let memory_requirements = unsafe { logical_device.get_image_memory_requirements(image) };

        let allocation = allocator
            .borrow_mut()
            .allocate(memory_requirements, MemoryLocation::GpuOnly)?;

        unsafe {
            logical_device
                .bind_image_memory(image, allocation.memory(), allocation.offset())
                .map_err(SwapchainCreationError::ImageMemoryBinding)?;
        }

        if !recreated {
            debug!("Binded depth image memory");
        }

        let subsource_range = ImageSubresourceRange::builder()
            .aspect_mask(ImageAspectFlags::DEPTH)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_view_create_info = ImageViewCreateInfo::builder()
            .image(image)
            .view_type(ImageViewType::TYPE_2D)
            .format(format)
            .components(ComponentMapping::default())
            .subresource_range(*subsource_range);

        let image_view = unsafe {
            logical_device
                .create_image_view(&image_view_create_info, None)
                .map_err(SwapchainCreationError::DepthImageViewCreation)?
        };

        if !recreated {
            debug!("Create depth image view");
        }

        Ok((image, image_view, format, allocation))
    }

    fn cleanup(&mut self) {
        unsafe {
            self.logical_device
                .destroy_image_view(self.depth_image_view, None);
            // TODO: Handle error
            self.allocator
                .borrow_mut()
                .free(self.depth_image_allocation.take().expect("FIXME"))
                .expect("FIXME");
            self.logical_device.destroy_image(self.depth_image, None);
            self.image_views
                .iter()
                .for_each(|image_view| self.logical_device.destroy_image_view(*image_view, None));
            self.swapchain_loader
                .destroy_swapchain(self.swapchain, None);
        }
    }

    pub fn recreate(&mut self, window: &Window) {
        // TODO: Handle error
        unsafe {
            self.logical_device.device_wait_idle().expect("FIXME");
        }

        self.cleanup();

        // TODO: Propagate error
        let (swapchain, extent, format) = Self::create_swapchain(
            window,
            &self.surface_loader,
            &self.surface,
            &self.physical_device,
            &self.swapchain_loader,
            true,
        )
        .expect("FIXME");

        self.swapchain = swapchain;

        self.extent = extent;
        self.format = format;

        // TODO: Propagate error
        let images = Self::create_images(&self.swapchain_loader, &swapchain, true).expect("FIXME");

        // TODO: Propagate error
        let image_views =
            Self::create_image_views(&self.logical_device, &format, &images, true).expect("FIXME");

        self.images = images;
        self.image_views = image_views;

        // TODO: Propagate error
        let (depth_image, depth_image_view, depth_format, depth_image_allocation) =
            Self::create_depth_image(window, &self.logical_device, &self.allocator, true)
                .expect("FIXME");

        self.depth_image = depth_image;
        self.depth_image_view = depth_image_view;
        self.depth_format = depth_format;
        self.depth_image_allocation = Some(depth_image_allocation);
    }

    pub fn swapchain_loader(&self) -> &SwapchainLoader {
        &self.swapchain_loader
    }

    pub fn swapchain(&self) -> &SwapchainKHR {
        &self.swapchain
    }

    pub fn extent(&self) -> &Extent2D {
        &self.extent
    }

    pub fn format(&self) -> &Format {
        &self.format
    }

    pub fn images(&self) -> &Vec<Image> {
        &self.images
    }

    pub fn image_views(&self) -> &Vec<ImageView> {
        &self.image_views
    }

    pub fn depth_image_view(&self) -> &ImageView {
        &self.depth_image_view
    }

    pub fn depth_format(&self) -> &Format {
        &self.depth_format
    }
}

impl Drop for Swapchain {
    fn drop(&mut self) {
        self.cleanup();
    }
}
