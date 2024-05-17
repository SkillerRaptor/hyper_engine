/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::sync::Arc;

use crate::{d3d12::GraphicsDevice, texture::TextureDescriptor};

struct TextureInner {
    graphics_device: GraphicsDevice,
}

pub(crate) struct Texture {
    inner: Arc<TextureInner>,
}

impl Texture {
    pub(crate) fn new(graphics_device: &GraphicsDevice, _descriptor: &TextureDescriptor) -> Self {
        todo!();

        Self {
            inner: Arc::new(TextureInner {
                graphics_device: graphics_device.clone(),
            }),
        }
    }

    pub(crate) fn view(&self) {
        todo!()
    }
}
