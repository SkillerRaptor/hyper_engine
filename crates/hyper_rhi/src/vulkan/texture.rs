//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use ash::vk;

use crate::{
    texture::TextureDescriptor,
    vulkan::graphics_device::{GraphicsDevice, ResourceHandler},
};

#[derive(Debug)]
pub(crate) struct Texture {
    height: u32,
    width: u32,

    view: vk::ImageView,
    image: vk::Image,

    resource_handler: Arc<ResourceHandler>,
}

impl Texture {
    pub(crate) fn new(
        _graphics_device: &GraphicsDevice,
        _resource_handler: &Arc<ResourceHandler>,
        _descriptor: &TextureDescriptor,
    ) -> Self {
        todo!();
    }

    // NOTE: Maybe add a descriptor for the format and more parameters?
    pub(super) fn new_external(
        graphics_device: &GraphicsDevice,
        resource_handler: &Arc<ResourceHandler>,
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

            view,
            image,

            resource_handler: Arc::clone(resource_handler),
        }
    }

    pub(crate) fn image(&self) -> vk::Image {
        self.image
    }

    pub(crate) fn view(&self) -> vk::ImageView {
        self.view
    }
}

impl Drop for Texture {
    fn drop(&mut self) {
        // TODO: Destroy texture allocation too if present

        self.resource_handler
            .texture_views
            .lock()
            .unwrap()
            .push(self.view);
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

impl crate::resource::Resource for Texture {
    fn resource_handle(&self) -> crate::resource::ResourceHandle {
        todo!()
    }
}
