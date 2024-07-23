//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use ash::vk;

use crate::{texture::TextureDescriptor, vulkan::GraphicsDevice};

struct TextureInner {
    height: u32,
    width: u32,

    view: vk::ImageView,
    image: vk::Image,

    graphics_device: GraphicsDevice,
}

#[derive(Clone)]
pub(crate) struct Texture {
    inner: Arc<TextureInner>,
}

impl Texture {
    pub(crate) fn new(_graphics_device: &GraphicsDevice, _descriptor: &TextureDescriptor) -> Self {
        todo!();
    }

    // NOTE: Maybe add a descriptor for the format and more parameters?
    pub(super) fn new_external(
        graphics_device: &GraphicsDevice,
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
        .expect("failed to create image view");

        Self {
            inner: Arc::new(TextureInner {
                height,
                width,

                view,
                image,

                graphics_device: graphics_device.clone(),
            }),
        }
    }

    pub(crate) fn image(&self) -> vk::Image {
        self.inner.image
    }

    pub(crate) fn view(&self) -> vk::ImageView {
        self.inner.view
    }

    pub(crate) fn width(&self) -> u32 {
        self.inner.width
    }

    pub(crate) fn height(&self) -> u32 {
        self.inner.height
    }
}
