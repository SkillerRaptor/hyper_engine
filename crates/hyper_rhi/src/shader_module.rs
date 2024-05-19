/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::fmt::{self, Display, Formatter};

use thiserror::Error;

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::vulkan;

#[derive(Debug, Error)]
pub enum ShaderModuleError {
    #[cfg(target_os = "windows")]
    #[error("failed to compile d3d12 shader")]
    D3D12(#[from] d3d12::ShaderModuleError),

    #[error("failed to compile vulkan shader")]
    Vulkan(#[from] vulkan::ShaderModuleError),
}

#[derive(Clone, Copy, Debug)]
pub enum ShaderStage {
    Compute,
    Pixel,
    Vertex,
}

impl Display for ShaderStage {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        match *self {
            ShaderStage::Compute => write!(f, "compute"),
            ShaderStage::Pixel => write!(f, "pixel"),
            ShaderStage::Vertex => write!(f, "vertex"),
        }
    }
}

#[derive(Clone, Debug)]
pub struct ShaderModuleDescriptor<'a> {
    pub path: &'a str,
    pub entry: &'a str,
    pub stage: ShaderStage,
}

enum ShaderModuleInner {
    #[cfg(target_os = "windows")]
    D3D12(d3d12::ShaderModule),
    Vulkan(vulkan::ShaderModule),
}

pub struct ShaderModule {
    inner: ShaderModuleInner,
}

impl ShaderModule {
    #[cfg(target_os = "windows")]
    pub(crate) fn new_d3d12(shader_module: d3d12::ShaderModule) -> Self {
        Self {
            inner: ShaderModuleInner::D3D12(shader_module),
        }
    }

    pub(crate) fn new_vulkan(shader_module: vulkan::ShaderModule) -> Self {
        Self {
            inner: ShaderModuleInner::Vulkan(shader_module),
        }
    }

    #[cfg(target_os = "windows")]
    pub(crate) fn d3d12_shader_module(&self) -> Option<&d3d12::ShaderModule> {
        match &self.inner {
            #[cfg(target_os = "windows")]
            ShaderModuleInner::D3D12(inner) => Some(inner),
            ShaderModuleInner::Vulkan(_) => None,
        }
    }

    pub(crate) fn vulkan_shader_module(&self) -> Option<&vulkan::ShaderModule> {
        match &self.inner {
            #[cfg(target_os = "windows")]
            ShaderModuleInner::D3D12(_) => None,
            ShaderModuleInner::Vulkan(inner) => Some(inner),
        }
    }
}
