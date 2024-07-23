//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use ash::vk;

use crate::{
    render_pass::RenderPassDescriptor,
    vulkan::{GraphicsDevice, RenderPass},
};

pub(crate) struct CommandList {
    graphics_device: GraphicsDevice,
}

impl CommandList {
    pub(crate) fn new(graphics_device: &GraphicsDevice) -> Self {
        let command_list = Self {
            graphics_device: graphics_device.clone(),
        };

        command_list.begin();

        command_list
    }

    pub(crate) fn begin(&self) {
        let command_buffer = self.graphics_device.current_frame().command_buffer;
        unsafe {
            self.graphics_device
                .device()
                .reset_command_buffer(command_buffer, vk::CommandBufferResetFlags::empty())
                .expect("failed to reset command buffer");

            let command_buffer_begin_info = vk::CommandBufferBeginInfo::default()
                .flags(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT);
            self.graphics_device
                .device()
                .begin_command_buffer(command_buffer, &command_buffer_begin_info)
                .expect("failed to begin command buffer");
        }
    }

    pub(crate) fn end(&self) {
        let command_buffer = self.graphics_device.current_frame().command_buffer;
        unsafe {
            self.graphics_device
                .device()
                .end_command_buffer(command_buffer)
                .expect("failed to end command buffer");
        }
    }

    pub(crate) fn begin_render_pass(&self, descriptor: &RenderPassDescriptor) -> RenderPass {
        RenderPass::new(&self.graphics_device, descriptor)
    }
}
