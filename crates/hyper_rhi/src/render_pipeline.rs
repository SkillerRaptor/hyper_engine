/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::vulkan;

#[derive(Clone, Debug)]
pub struct RenderPipelineDescriptor<'a> {
    pub vertex_shader: &'a str,
    pub pixel_shader: &'a str,
}

enum RenderPipelineInner {
    #[cfg(target_os = "windows")]
    D3D12(d3d12::RenderPipeline),
    Vulkan(vulkan::RenderPipeline),
}

pub struct RenderPipeline {
    inner: RenderPipelineInner,
}

impl RenderPipeline {
    #[cfg(target_os = "windows")]
    pub(crate) fn new_d3d12(render_pipeline: d3d12::RenderPipeline) -> Self {
        Self {
            inner: RenderPipelineInner::D3D12(render_pipeline),
        }
    }

    pub(crate) fn new_vulkan(render_pipeline: vulkan::RenderPipeline) -> Self {
        Self {
            inner: RenderPipelineInner::Vulkan(render_pipeline),
        }
    }

    #[cfg(target_os = "windows")]
    pub(crate) fn d3d12_render_pipeline(&self) -> &d3d12::RenderPipeline {
        let RenderPipelineInner::D3D12(render_pipeline) = &self.inner else {
            panic!()
        };

        render_pipeline
    }

    pub(crate) fn vulkan_render_pipeline(&self) -> &vulkan::RenderPipeline {
        let RenderPipelineInner::Vulkan(render_pipeline) = &self.inner else {
            panic!()
        };

        render_pipeline
    }
}
