//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{fmt::Debug, sync::Arc};

use crate::{
    buffer::Buffer,
    commands::command::Command,
    graphics_pipeline::GraphicsPipeline,
    texture::Texture,
};

#[derive(Clone, Debug)]
pub struct RenderPassDescriptor<'a> {
    pub label: Option<&'a str>,
    pub texture: &'a Arc<dyn Texture>,
}

pub struct RenderPass<'a> {
    commands: &'a mut Vec<Command>,
}

impl<'a> RenderPass<'a> {
    pub(crate) fn new(commands: &'a mut Vec<Command>, descriptor: &RenderPassDescriptor) -> Self {
        commands.push(Command::BeginRenderPass {
            texture: Arc::clone(descriptor.texture),
        });

        Self { commands }
    }

    pub fn push_constants(&mut self, data: &[u8]) {
        self.commands.push(Command::PushConstants {
            data: data.to_vec(),
        })
    }

    pub fn bind_pipeline(&mut self, pipeline: &Arc<dyn GraphicsPipeline>) {
        self.commands.push(Command::BindGraphicsPipeline {
            graphics_pipeline: Arc::clone(pipeline),
        });
    }

    pub fn bind_index_buffer(&mut self, buffer: &Arc<dyn Buffer>) {
        self.commands.push(Command::BindIndexBuffer {
            buffer: Arc::clone(buffer),
        });
    }

    pub fn draw(
        &mut self,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        self.commands.push(Command::Draw {
            vertex_count,
            instance_count,
            first_vertex,
            first_instance,
        });
    }

    pub fn draw_indexed(
        &mut self,
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    ) {
        self.commands.push(Command::DrawIndexed {
            index_count,
            instance_count,
            first_index,
            vertex_offset,
            first_instance,
        });
    }
}

impl<'a> Drop for RenderPass<'a> {
    fn drop(&mut self) {
        self.commands.push(Command::EndRenderPass);
    }
}
