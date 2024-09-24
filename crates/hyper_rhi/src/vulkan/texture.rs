//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    fmt::{self, Debug, Formatter},
    mem,
    sync::Arc,
};

use ash::vk;
use gpu_allocator::vulkan::Allocation;

use crate::{
    resource::{Resource, ResourceHandle},
    texture::TextureDescriptor,
    vulkan::graphics_device::GraphicsDeviceShared,
};

pub(crate) struct Texture {
    height: u32,
    width: u32,

    allocation: Option<Allocation>,
    view: vk::ImageView,
    raw: vk::Image,

    graphics_device: Arc<GraphicsDeviceShared>,
}

impl Texture {
    pub(crate) fn new(
        _graphics_device: &Arc<GraphicsDeviceShared>,
        _descriptor: &TextureDescriptor,
    ) -> Self {
        todo!();
    }

    // NOTE: Maybe add a descriptor for the format and more parameters?
    pub(super) fn new_external(
        graphics_device: &Arc<GraphicsDeviceShared>,
        image: vk::Image,
        width: u32,
        height: u32,
        format: vk::Format,
    ) -> Self {
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
            .format(format)
            .components(component_mapping)
            .subresource_range(subresource_range);

        let view = unsafe {
            graphics_device
                .device()
                .create_image_view(&create_info, None)
        }
        .unwrap();

        Self {
            height,
            width,

            allocation: None,
            view,
            raw: image,

            graphics_device: Arc::clone(graphics_device),
        }
    }

    pub(crate) fn image(&self) -> vk::Image {
        self.raw
    }

    pub(crate) fn view(&self) -> vk::ImageView {
        self.view
    }
}

impl Drop for Texture {
    fn drop(&mut self) {
        self.graphics_device.resource_queue().push_texture(
            self.raw,
            self.view,
            mem::take(&mut self.allocation),
        );
    }
}

impl Debug for Texture {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        f.debug_struct("Texture")
            .field("height", &self.height)
            .field("width", &self.width)
            .field("view", &self.view)
            .field("raw", &self.raw)
            .finish()
    }
}

impl crate::texture::Texture for Texture {
    fn width(&self) -> u32 {
        self.width
    }

    fn height(&self) -> u32 {
        self.height
    }
}

impl Resource for Texture {
    fn handle(&self) -> ResourceHandle {
        todo!()
    }
}
