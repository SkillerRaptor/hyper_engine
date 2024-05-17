/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::fmt::Debug;

use crate::{render_pipeline::RenderPipeline, texture_view::TextureView};

#[derive(Clone)]
pub struct RenderPassDescriptor<'a> {
    // TODO: Add image view type
    pub image_view: &'a TextureView<'a>,
}

impl<'a> Debug for RenderPassDescriptor<'a> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("RenderPassDescriptor")
            .finish_non_exhaustive()
    }
}

pub struct RenderPass {}

impl RenderPass {
    pub fn bind_pipeline(&mut self, _pipeline: &RenderPipeline) {
        todo!()
    }

    pub fn draw(
        &mut self,
        _vertex_count: u32,
        _instance_count: u32,
        _first_vertex: u32,
        _first_instance: u32,
    ) {
        todo!()
    }
}

impl Drop for RenderPass {
    fn drop(&mut self) {
        todo!()
    }
}
