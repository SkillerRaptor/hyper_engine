/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::device::DeviceShared;

use ash::vk;
use color_eyre::eyre::Result;

use std::sync::Arc;

pub struct Image {
    format: vk::Format,
    height: u32,
    width: u32,
    view: vk::ImageView,
    raw: vk::Image,
    device: Arc<DeviceShared>,
}

impl Image {
    pub(crate) fn new() -> Result<Self> {
        // TODO: Add a way to distinguish an external from internal image
        todo!();
    }

    pub(crate) fn new_external(
        device: &Arc<DeviceShared>,
        raw: vk::Image,
        width: u32,
        height: u32,
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
            format,
            height,
            width,
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

    pub(crate) fn width(&self) -> u32 {
        self.width
    }

    pub(crate) fn height(&self) -> u32 {
        self.height
    }

    pub(crate) fn format(&self) -> vk::Format {
        self.format
    }
}

impl Drop for Image {
    fn drop(&mut self) {
        unsafe {
            self.device.raw().destroy_image_view(self.view, None);
        }
    }
}

#[derive(Clone, Copy)]
pub enum ImageLayout {
    Undefined,
    ColorAttachment,
    PresentSrc,
}

impl Into<vk::ImageLayout> for ImageLayout {
    fn into(self) -> vk::ImageLayout {
        match self {
            ImageLayout::Undefined => vk::ImageLayout::UNDEFINED,
            ImageLayout::ColorAttachment => vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
            ImageLayout::PresentSrc => vk::ImageLayout::PRESENT_SRC_KHR,
        }
    }
}

impl Into<vk::AccessFlags2> for ImageLayout {
    fn into(self) -> vk::AccessFlags2 {
        match self {
            ImageLayout::Undefined => vk::AccessFlags2::empty(),
            ImageLayout::ColorAttachment => vk::AccessFlags2::COLOR_ATTACHMENT_WRITE,
            ImageLayout::PresentSrc => vk::AccessFlags2::empty(),
        }
    }
}

impl Into<vk::PipelineStageFlags2> for ImageLayout {
    fn into(self) -> vk::PipelineStageFlags2 {
        match self {
            ImageLayout::Undefined => vk::PipelineStageFlags2::TOP_OF_PIPE,
            ImageLayout::ColorAttachment => vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT,
            ImageLayout::PresentSrc => vk::PipelineStageFlags2::BOTTOM_OF_PIPE,
        }
    }
}
