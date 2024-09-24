//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use crate::{
    d3d12::{graphics_device::GraphicsDeviceShared, render_pass::RenderPass},
    render_pass::RenderPassDescriptor,
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
        let command_allocator = &self.graphics_device.current_frame().command_allocator;

        unsafe {
            command_allocator.Reset().unwrap();
        }
    }

    fn end(&self) {
        let command_list = &self.graphics_device.current_frame().command_list;

        unsafe {
            command_list.Close().unwrap();
        }
    }

    fn begin_render_pass(
        &self,
        descriptor: &RenderPassDescriptor,
    ) -> Arc<dyn crate::render_pass::RenderPass> {
        Arc::new(RenderPass::new(&self.graphics_device, descriptor))
    }
}
