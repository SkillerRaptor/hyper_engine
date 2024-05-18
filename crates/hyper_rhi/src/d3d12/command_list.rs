/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use crate::{
    d3d12::{GraphicsDevice, RenderPass},
    render_pass::RenderPassDescriptor,
};

pub(crate) struct CommandList {
    graphics_device: GraphicsDevice,
}

impl CommandList {
    // NOTE: Maybe store the current command list?
    pub(crate) fn new(graphics_device: &GraphicsDevice) -> Self {
        let command_list = Self {
            graphics_device: graphics_device.clone(),
        };

        command_list.begin();

        command_list
    }

    pub(crate) fn begin(&self) {
        let current_frame = self.graphics_device.current_frame();
        let command_allocator = &current_frame.command_allocator;

        unsafe {
            command_allocator
                .Reset()
                .expect("failed to reset command allocator");
        }
    }

    pub(crate) fn end(&self) {
        let current_frame = self.graphics_device.current_frame();
        let command_list = &current_frame.command_list;

        unsafe {
            command_list.Close().expect("failed to close command list");
        }
    }

    pub(crate) fn begin_render_pass(&self, descriptor: &RenderPassDescriptor) -> RenderPass {
        RenderPass::new(&self.graphics_device, descriptor)
    }
}
