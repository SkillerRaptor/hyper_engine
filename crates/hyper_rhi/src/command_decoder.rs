//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use crate::{graphics_pipeline::GraphicsPipeline, texture::Texture};

pub trait CommandDecoder {
    fn begin_render_pass(&self, texture: &dyn Texture);
    fn end_render_pass(&self, texture: &dyn Texture);

    fn bind_pipeline(
        &self,
        graphics_pipeline: &dyn GraphicsPipeline,
        texture: &dyn crate::texture::Texture,
    );

    fn draw(&self, vertex_count: u32, instance_count: u32, first_vertex: u32, first_instance: u32);
}
