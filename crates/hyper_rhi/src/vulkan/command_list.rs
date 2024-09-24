//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use ash::vk;

use crate::{
    render_pass::RenderPassDescriptor,
    vulkan::{graphics_device::GraphicsDeviceShared, render_pass::RenderPass},
};

pub(crate) struct CommandList {
    graphics_device: Arc<GraphicsDeviceShared>,
}

impl CommandList {
    pub(crate) fn new(graphics_device: &Arc<GraphicsDeviceShared>) -> Self {
        Self {
            graphics_device: Arc::clone(graphics_device),
        }
    }
}

impl crate::command_list::CommandList for CommandList {
    fn begin(&self) {
        let command_buffer = self.graphics_device.current_frame().command_buffer;

        unsafe {
            self.graphics_device
                .device()
                .reset_command_buffer(command_buffer, vk::CommandBufferResetFlags::empty())
                .unwrap();

            let command_buffer_begin_info = vk::CommandBufferBeginInfo::default()
                .flags(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT);
            self.graphics_device
                .device()
                .begin_command_buffer(command_buffer, &command_buffer_begin_info)
                .unwrap();
        }
    }

    fn end(&self) {
        let command_buffer = self.graphics_device.current_frame().command_buffer;

        unsafe {
            self.graphics_device
                .device()
                .end_command_buffer(command_buffer)
                .unwrap();
        }
    }

    fn begin_render_pass(
        &self,
        descriptor: &RenderPassDescriptor,
    ) -> Arc<dyn crate::render_pass::RenderPass> {
        Arc::new(RenderPass::new(&self.graphics_device, descriptor))
    }
}
