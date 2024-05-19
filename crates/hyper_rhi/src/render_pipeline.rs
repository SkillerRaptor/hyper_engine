/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::{shader_module::ShaderModule, vulkan};

#[derive(Clone)]
pub struct RenderPipelineDescriptor<'a> {
    pub vertex_shader: &'a ShaderModule,
    pub pixel_shader: &'a ShaderModule,
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
    pub(crate) fn d3d12_render_pipeline(&self) -> Option<&d3d12::RenderPipeline> {
        match &self.inner {
            RenderPipelineInner::D3D12(inner) => Some(inner),
            RenderPipelineInner::Vulkan(_) => None,
        }
    }

    pub(crate) fn vulkan_render_pipeline(&self) -> Option<&vulkan::RenderPipeline> {
        match &self.inner {
            RenderPipelineInner::D3D12(_) => None,
            RenderPipelineInner::Vulkan(inner) => Some(inner),
        }
    }
}
