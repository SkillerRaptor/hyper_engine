/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use crate::{render_pass::RenderPassDescriptor, vulkan::RenderPipeline};

pub(crate) struct RenderPass {}

impl RenderPass {
    pub(crate) fn new(descriptor: &RenderPassDescriptor) -> Self {
        Self {}
    }

    pub(crate) fn bind_pipeline(&self, pipeline: &RenderPipeline) {
        todo!()
    }

    pub(crate) fn draw(
        &self,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        todo!()
    }
}
