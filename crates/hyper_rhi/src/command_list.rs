//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use crate::{graphics_pipeline::GraphicsPipeline, texture::Texture};

#[derive(Clone, Debug)]
pub(crate) enum Command {
    BeginRenderPass {
        texture: Arc<dyn Texture>,
    },
    EndRenderPass,

    BindPipeline {
        graphics_pipeline: Arc<dyn GraphicsPipeline>,
    },

    Draw {
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    },
}

pub struct CommandList(Vec<Command>);

impl CommandList {
    pub(crate) fn new(commands: Vec<Command>) -> Self {
        Self(commands)
    }

    pub(crate) fn commands(&self) -> &[Command] {
        &self.0
    }
}
