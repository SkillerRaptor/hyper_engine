//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use crate::{buffer::Buffer, graphics_pipeline::GraphicsPipeline, texture::Texture};

pub trait CommandDecoder {
    fn begin_render_pass(&self, texture: &Arc<dyn Texture>);
    fn end_render_pass(&self, texture: &Arc<dyn Texture>);

    fn bind_descriptor(&self, buffer: &Arc<dyn Buffer>);

    fn bind_pipeline(
        &self,
        graphics_pipeline: &Arc<dyn GraphicsPipeline>,
        texture: &Arc<dyn Texture>,
    );
    fn bind_index_buffer(&self, buffer: &Arc<dyn Buffer>);

    fn draw(&self, vertex_count: u32, instance_count: u32, first_vertex: u32, first_instance: u32);
    fn draw_indexed(
        &self,
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    );
}
