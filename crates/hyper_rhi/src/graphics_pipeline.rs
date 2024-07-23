//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::{shader_module::ShaderModule, vulkan};

#[derive(Clone)]
pub struct GraphicsPipelineDescriptor<'a> {
    pub vertex_shader: &'a ShaderModule,
    pub pixel_shader: &'a ShaderModule,
}

enum GraphicsPipelineInner {
    #[cfg(target_os = "windows")]
    D3D12(d3d12::GraphicsPipeline),
    Vulkan(vulkan::GraphicsPipeline),
}

pub struct GraphicsPipeline {
    inner: GraphicsPipelineInner,
}

impl GraphicsPipeline {
    #[cfg(target_os = "windows")]
    pub(crate) fn new_d3d12(graphics_pipeline: d3d12::GraphicsPipeline) -> Self {
        Self {
            inner: GraphicsPipelineInner::D3D12(graphics_pipeline),
        }
    }

    pub(crate) fn new_vulkan(graphics_pipeline: vulkan::GraphicsPipeline) -> Self {
        Self {
            inner: GraphicsPipelineInner::Vulkan(graphics_pipeline),
        }
    }

    #[cfg(target_os = "windows")]
    pub(crate) fn d3d12_graphics_pipeline(&self) -> Option<&d3d12::GraphicsPipeline> {
        match &self.inner {
            #[cfg(target_os = "windows")]
            GraphicsPipelineInner::D3D12(inner) => Some(inner),
            GraphicsPipelineInner::Vulkan(_) => None,
        }
    }

    pub(crate) fn vulkan_graphics_pipeline(&self) -> Option<&vulkan::GraphicsPipeline> {
        match &self.inner {
            #[cfg(target_os = "windows")]
            GraphicsPipelineInner::D3D12(_) => None,
            GraphicsPipelineInner::Vulkan(inner) => Some(inner),
        }
    }
}
