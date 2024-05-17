/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::render_pipeline::RenderPipeline;

#[derive(Clone, Debug)]
pub struct RenderPassDescriptor {
    // TODO: Add image view type
    pub image_view: (),
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
