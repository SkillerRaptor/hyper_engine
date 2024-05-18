/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::sync::Arc;

use ash::vk;

use crate::{
    texture::TextureDescriptor,
    vulkan::{GraphicsDevice, TextureView, TextureViewDescriptor},
};

struct TextureInner {
    view: TextureView,
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
        format: vk::Format,
    ) -> Self {
        let view = TextureView::new(graphics_device, image, &TextureViewDescriptor { format });

        Self {
            inner: Arc::new(TextureInner {
                view,
                image,

                graphics_device: graphics_device.clone(),
            }),
        }
    }

    pub(crate) fn image(&self) -> vk::Image {
        self.inner.image
    }

    pub(crate) fn view(&self) -> &TextureView {
        &self.inner.view
    }
}
