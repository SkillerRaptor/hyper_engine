/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{device::DeviceShared, surface::Surface};

use ash::{
    extensions::khr::Swapchain as SwapchainFunctor,
    vk::{
        ColorSpaceKHR, ComponentMapping, ComponentSwizzle, CompositeAlphaFlagsKHR, Extent2D,
        Format, Image, ImageAspectFlags, ImageSubresourceRange, ImageUsageFlags, ImageView,
        ImageViewCreateInfo, ImageViewType, PresentModeKHR, SharingMode, SurfaceCapabilitiesKHR,
        SurfaceFormatKHR, SwapchainCreateInfoKHR, SwapchainKHR,
    },
};
use color_eyre::Result;
use std::sync::Arc;

#[derive(Debug)]
pub(super) struct SwapchainDescriptor {
    pub(super) width: u32,
    pub(super) height: u32,
}

impl Default for SwapchainDescriptor {
    fn default() -> Self {
        Self {
            width: 1280,
            height: 720,
        }
    }
}

pub(super) struct Swapchain {
    image_views: Vec<ImageView>,
    images: Vec<Image>,
    raw: SwapchainKHR,
    functor: SwapchainFunctor,
    device: Arc<DeviceShared>,
}

impl Swapchain {
    pub(super) fn new(
        surface: &Surface,
        device: Arc<DeviceShared>,
        descriptor: &SwapchainDescriptor,
    ) -> Result<Self> {
        let surface_details = device.query_surface_details(surface)?;

        let extent = Self::choose_extent(descriptor, surface_details.capabilities());
        let surface_format = Self::choose_surface_format(surface_details.formats());
        let present_mode = Self::choose_present_mode(surface_details.present_modes());

        let functor = SwapchainFunctor::new(device.instance().raw(), device.logical_device());
        let raw = Self::create_swapchain(
            surface,
            surface_details.capabilities(),
            extent,
            surface_format,
            present_mode,
            &functor,
            None,
        )?;

        let (images, image_views) =
            Self::create_images_and_views(&device, surface_format, &functor, raw)?;

        Ok(Self {
            image_views,
            images,
            raw,
            functor,
            device,
        })
    }

    fn choose_extent(
        descriptor: &SwapchainDescriptor,
        capabilities: &SurfaceCapabilitiesKHR,
    ) -> Extent2D {
        if capabilities.current_extent.width != u32::MAX
            || capabilities.current_extent.height != u32::MAX
        {
            return capabilities.current_extent;
        }

        Extent2D {
            width: descriptor.width.clamp(
                capabilities.min_image_extent.width,
                capabilities.max_image_extent.width,
            ),
            height: descriptor.height.clamp(
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

    fn create_swapchain(
        surface: &Surface,
        capabilities: &SurfaceCapabilitiesKHR,
        extent: Extent2D,
        surface_format: SurfaceFormatKHR,
        present_mode: PresentModeKHR,
        functor: &SwapchainFunctor,
        old_swapchain: Option<SwapchainKHR>,
    ) -> Result<SwapchainKHR> {
        let mut image_count = capabilities.min_image_count + 1;
        if capabilities.max_image_count > 0 && image_count > capabilities.max_image_count {
            image_count = capabilities.max_image_count;
        }

        let create_info = SwapchainCreateInfoKHR::builder()
            .surface(surface.raw())
            .min_image_count(image_count)
            .image_format(surface_format.format)
            .image_color_space(surface_format.color_space)
            .image_extent(extent)
            .image_array_layers(1)
            .image_usage(ImageUsageFlags::COLOR_ATTACHMENT)
            .image_sharing_mode(SharingMode::EXCLUSIVE)
            .queue_family_indices(&[])
            .pre_transform(capabilities.current_transform)
            .composite_alpha(CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(present_mode)
            .clipped(true)
            .old_swapchain(old_swapchain.unwrap_or(SwapchainKHR::null()));

        let raw = unsafe { functor.create_swapchain(&create_info, None) }?;
        Ok(raw)
    }

    fn create_images_and_views(
        device: &DeviceShared,
        surface_format: SurfaceFormatKHR,
        functor: &SwapchainFunctor,
        raw: SwapchainKHR,
    ) -> Result<(Vec<Image>, Vec<ImageView>)> {
        let images = unsafe { functor.get_swapchain_images(raw) }?;

        let image_views = images
            .iter()
            .map(|image| {
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

                let raw = unsafe {
                    device
                        .logical_device()
                        .create_image_view(&create_info, None)
                }?;
                Ok(raw)
            })
            .collect::<Result<Vec<_>>>()?;

        Ok((images, image_views))
    }

    pub(super) fn raw(&self) -> &SwapchainKHR {
        &self.raw
    }
}

impl Drop for Swapchain {
    fn drop(&mut self) {
        unsafe {
            self.image_views.iter().for_each(|&image_view| {
                self.device
                    .logical_device()
                    .destroy_image_view(image_view, None);
            });

            self.functor.destroy_swapchain(self.raw, None);
        }
    }
}
