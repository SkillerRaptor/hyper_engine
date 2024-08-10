//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{fmt::Debug, sync::Arc};

use crate::{command_list::Command, graphics_pipeline::GraphicsPipeline, texture::Texture};

#[derive(Clone, Debug)]
pub struct RenderPassDescriptor<'a> {
    // TODO: Add image view type
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

    pub fn bind_pipeline(&mut self, pipeline: &Arc<dyn GraphicsPipeline>) {
        self.commands.push(Command::BindPipeline {
            graphics_pipeline: Arc::clone(pipeline),
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
}

impl<'a> Drop for RenderPass<'a> {
    fn drop(&mut self) {
        self.commands.push(Command::EndRenderPass);
    }
}
