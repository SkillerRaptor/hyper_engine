//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use crate::{buffer::Buffer, graphics_pipeline::GraphicsPipeline, texture::Texture};

#[derive(Clone, Debug)]
pub(crate) enum Command {
    // Pass Commands
    BeginRenderPass {
        texture: Arc<dyn Texture>,
    },
    EndRenderPass,

    // Bind Commands
    BindDescriptor {
        buffer: Arc<dyn Buffer>,
    },

    BindGraphicsPipeline {
        graphics_pipeline: Arc<dyn GraphicsPipeline>,
    },

    BindIndexBuffer {
        buffer: Arc<dyn Buffer>,
    },

    // Draw commands
    Draw {
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    },

    DrawIndexed {
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    },
}
