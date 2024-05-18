/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use crate::{
    render_pass::RenderPassDescriptor,
    vulkan::{GraphicsDevice, RenderPass},
};

pub(crate) struct CommandList {
    graphics_device: GraphicsDevice,
}

impl CommandList {
    pub(crate) fn new(graphics_device: &GraphicsDevice) -> Self {
        Self {
            graphics_device: graphics_device.clone(),
        }
    }

    pub(crate) fn begin(&self) {
        todo!()
    }

    pub(crate) fn end(&self) {
        todo!()
    }

    pub(crate) fn begin_render_pass(&self, descriptor: &RenderPassDescriptor) -> RenderPass {
        todo!()
    }
}
