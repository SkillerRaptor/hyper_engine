/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::sync::Arc;

use ash::vk;
use color_eyre::eyre::Result;

use crate::device::DeviceShared;

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
