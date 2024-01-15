/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::device::DeviceShared;

use ash::vk;
use color_eyre::eyre::Result;
use gpu_allocator::{
    vulkan::{Allocation, AllocationCreateDesc, AllocationScheme},
    MemoryLocation,
};

use std::sync::Arc;

// TODO: Remove the most and replace with generic attachment and read only
#[derive(Clone, Copy)]
pub enum ImageLayout {
    Undefined,
    General,
    ColorAttachment,
    DepthStencilAttachment,
    DepthStencilReadOnly,
    ShaderReadOnly,
    TransferSource,
    TransferDestination,
    PreInitialized,
    DepthReadOnlyStencilAttachment,
    DepthAttachmentStencilReadOnly,
    DepthAttachment,
    DepthReadOnly,
    StencilAttachment,
    StencilReadOnly,
    ReadOnly,
    Attachment,
    PresentSource,
}

impl From<ImageLayout> for vk::ImageLayout {
    fn from(value: ImageLayout) -> Self {
        match value {
            ImageLayout::Undefined => vk::ImageLayout::UNDEFINED,
            ImageLayout::General => vk::ImageLayout::GENERAL,
            ImageLayout::ColorAttachment => vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
            ImageLayout::DepthStencilAttachment => {
                vk::ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            }
            ImageLayout::DepthStencilReadOnly => vk::ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            ImageLayout::ShaderReadOnly => vk::ImageLayout::SHADER_READ_ONLY_OPTIMAL,
            ImageLayout::TransferSource => vk::ImageLayout::TRANSFER_SRC_OPTIMAL,
            ImageLayout::TransferDestination => vk::ImageLayout::TRANSFER_DST_OPTIMAL,
            ImageLayout::PreInitialized => vk::ImageLayout::PREINITIALIZED,
            ImageLayout::DepthReadOnlyStencilAttachment => {
                vk::ImageLayout::DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL
            }
            ImageLayout::DepthAttachmentStencilReadOnly => {
                vk::ImageLayout::DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL
            }
            ImageLayout::DepthAttachment => vk::ImageLayout::DEPTH_ATTACHMENT_OPTIMAL,
            ImageLayout::DepthReadOnly => vk::ImageLayout::DEPTH_READ_ONLY_OPTIMAL,
            ImageLayout::StencilAttachment => vk::ImageLayout::STENCIL_ATTACHMENT_OPTIMAL,
            ImageLayout::StencilReadOnly => vk::ImageLayout::STENCIL_READ_ONLY_OPTIMAL,
            ImageLayout::ReadOnly => vk::ImageLayout::READ_ONLY_OPTIMAL,
            ImageLayout::Attachment => vk::ImageLayout::ATTACHMENT_OPTIMAL,
            ImageLayout::PresentSource => vk::ImageLayout::PRESENT_SRC_KHR,
        }
    }
}

#[derive(Clone, Copy)]
pub enum ImageFormat {
    // TODO: Maybe improve name?
    Rgba16Sfloat,
}

impl From<ImageFormat> for vk::Format {
    fn from(value: ImageFormat) -> Self {
        match value {
            ImageFormat::Rgba16Sfloat => vk::Format::R16G16B16A16_SFLOAT,
        }
    }
}

bitflags::bitflags! {
    #[derive(Clone, Copy)]
    pub struct ImageUsage: u32 {
        const TRANSFER_SOURCE = 0b1;
        const TRANSFER_DESTINATION = 0b10;
        const SAMPLED = 0b100;
        const STORAGE = 0b1000;
        const COLOR_ATTACHMENT = 0b1_0000;
        const DEPTH_STENCIL_ATTACHMENT = 0b10_0000;
        const TRANSIENT_ATTACHMENT = 0b100_0000;
        const INPUT_ATTACHMENT = 0b1000_0000;
    }
}

impl From<ImageUsage> for vk::ImageUsageFlags {
    fn from(value: ImageUsage) -> Self {
        vk::ImageUsageFlags::from_raw(value.bits())
    }
}

// TODO: Add default
pub struct ImageDescriptor {
    pub width: u32,
    pub height: u32,
    pub format: ImageFormat,
    pub usage: ImageUsage,
}

pub struct Image {
    _format: vk::Format,
    extent: vk::Extent2D,
    allocation: Option<Allocation>,
    view: vk::ImageView,
    raw: vk::Image,
    device: Arc<DeviceShared>,
}

impl Image {
    pub(crate) fn new(device: &Arc<DeviceShared>, descriptor: ImageDescriptor) -> Result<Self> {
        let create_info = vk::ImageCreateInfo::builder()
            .image_type(vk::ImageType::TYPE_2D)
            .format(descriptor.format.into())
            .extent(vk::Extent3D {
                width: descriptor.width,
                height: descriptor.height,
                depth: 1,
            })
            .mip_levels(1)
            .array_layers(1)
            .samples(vk::SampleCountFlags::TYPE_1)
            .tiling(vk::ImageTiling::OPTIMAL)
            .usage(descriptor.usage.into())
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[])
            .initial_layout(vk::ImageLayout::UNDEFINED);

        let image = unsafe { device.raw().create_image(&create_info, None) }?;

        // TODO: Make this more variable with creation
        let requirements = unsafe { device.raw().get_image_memory_requirements(image) };
        let allocation = device
            .allocator()
            .lock()
            .unwrap()
            .allocate(&AllocationCreateDesc {
                name: "",
                requirements,
                location: MemoryLocation::GpuOnly,
                linear: false,
                allocation_scheme: AllocationScheme::DedicatedImage(image),
            })?;

        unsafe {
            device
                .raw()
                .bind_image_memory(image, allocation.memory(), allocation.offset())
        }?;

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
            .image(image)
            .view_type(vk::ImageViewType::TYPE_2D)
            .format(descriptor.format.into())
            .components(*component_mapping)
            .subresource_range(*subresource_range);

        let view = unsafe { device.raw().create_image_view(&create_info, None) }?;

        Ok(Self {
            _format: descriptor.format.into(),
            extent: vk::Extent2D {
                width: descriptor.width,
                height: descriptor.height,
            },
            allocation: Some(allocation),
            view,
            raw: image,
            device: Arc::clone(device),
        })
    }

    pub(crate) fn new_external(
        device: &Arc<DeviceShared>,
        raw: vk::Image,
        extent: vk::Extent2D,
        format: vk::Format,
    ) -> Result<Self> {
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
            .image(raw)
            .view_type(vk::ImageViewType::TYPE_2D)
            .format(format)
            .components(*component_mapping)
            .subresource_range(*subresource_range);

        let view = unsafe { device.raw().create_image_view(&create_info, None) }?;

        Ok(Self {
            _format: format,
            extent,
            allocation: None,
            view,
            raw,
            device: Arc::clone(device),
        })
    }

    pub(crate) fn raw(&self) -> vk::Image {
        self.raw
    }

    pub(crate) fn view(&self) -> vk::ImageView {
        self.view
    }

    pub fn width(&self) -> u32 {
        self.extent.width
    }

    pub fn height(&self) -> u32 {
        self.extent.height
    }
}

impl Drop for Image {
    fn drop(&mut self) {
        unsafe {
            self.device.raw().destroy_image_view(self.view, None);

            if let Some(allocation) = self.allocation.take() {
                self.device
                    .allocator()
                    .lock()
                    .unwrap()
                    .free(allocation)
                    .unwrap();
                self.device.raw().destroy_image(self.raw, None);
            }
        }
    }
}
