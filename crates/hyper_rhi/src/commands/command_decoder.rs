//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use crate::{
    buffer::Buffer,
    graphics_pipeline::GraphicsPipeline,
    resource::ResourceHandle,
    texture::Texture,
};

pub trait CommandDecoder {
    fn begin_render_pass(&self, texture: &dyn Texture);
    fn end_render_pass(&self, texture: &dyn Texture);

    fn bind_descriptor(&self, buffer: ResourceHandle);

    fn bind_pipeline(&self, graphics_pipeline: &dyn GraphicsPipeline, texture: &dyn Texture);
    fn bind_index_buffer(&self, buffer: &dyn Buffer);

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
