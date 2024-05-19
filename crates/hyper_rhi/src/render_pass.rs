/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::fmt::Debug;

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::{render_pipeline::RenderPipeline, texture::Texture, vulkan};

#[derive(Clone)]
pub struct RenderPassDescriptor<'a> {
    // TODO: Add image view type
    pub texture: &'a Texture,
}

impl<'a> Debug for RenderPassDescriptor<'a> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("RenderPassDescriptor")
            .finish_non_exhaustive()
    }
}

enum RenderPassInner {
    #[cfg(target_os = "windows")]
    D3D12(d3d12::RenderPass),
    Vulkan(vulkan::RenderPass),
}

pub struct RenderPass {
    inner: RenderPassInner,
}

impl RenderPass {
    #[cfg(target_os = "windows")]
    pub(crate) fn new_d3d12(render_pass: d3d12::RenderPass) -> Self {
        Self {
            inner: RenderPassInner::D3D12(render_pass),
        }
    }

    pub(crate) fn new_vulkan(render_pass: vulkan::RenderPass) -> Self {
        Self {
            inner: RenderPassInner::Vulkan(render_pass),
        }
    }

    pub fn bind_pipeline(&self, pipeline: &RenderPipeline) {
        match &self.inner {
            #[cfg(target_os = "windows")]
            RenderPassInner::D3D12(inner) => inner.bind_pipeline(pipeline),
            RenderPassInner::Vulkan(inner) => inner.bind_pipeline(pipeline),
        }
    }

    pub fn draw(
        &self,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        match &self.inner {
            #[cfg(target_os = "windows")]
            RenderPassInner::D3D12(inner) => {
                inner.draw(vertex_count, instance_count, first_vertex, first_instance)
            }
            RenderPassInner::Vulkan(inner) => {
                inner.draw(vertex_count, instance_count, first_vertex, first_instance)
            }
        }
    }
}
