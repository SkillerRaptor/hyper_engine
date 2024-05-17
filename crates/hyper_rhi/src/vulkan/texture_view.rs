/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use ash::vk;

use crate::vulkan::GraphicsDevice;

#[derive(Clone, Debug)]
pub(super) struct TextureViewDescriptor {
    pub(super) format: vk::Format,
}

pub(crate) struct TextureView {
    view: vk::ImageView,

    graphics_device: GraphicsDevice,
}

impl TextureView {
    pub(super) fn new(
        graphics_device: &GraphicsDevice,
        image: vk::Image,
        descriptor: &TextureViewDescriptor,
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
            .format(descriptor.format)
            .components(component_mapping)
            .subresource_range(subresource_range);

        let view = unsafe {
            graphics_device
                .device()
                .create_image_view(&create_info, None)
        }
        .expect("failed to create image view");

        Self {
            view,

            graphics_device: graphics_device.clone(),
        }
    }
}

impl Drop for TextureView {
    fn drop(&mut self) {
        unsafe {
            self.graphics_device
                .device()
                .destroy_image_view(self.view, None);
        }
    }
}
