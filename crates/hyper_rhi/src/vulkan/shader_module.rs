//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use ash::vk;
use thiserror::Error;

use crate::{
    shader_compiler::{self, OutputApi, ShaderCompilationError},
    shader_module::{ShaderModuleDescriptor, ShaderStage},
    vulkan::GraphicsDevice,
};

#[derive(Debug, Error)]
pub enum ShaderModuleError {
    #[error("failed to compile {stage} shader at '{path}'")]
    Compilation {
        #[source]
        source: ShaderCompilationError,
        path: String,
        stage: ShaderStage,
    },

    #[error("failed to align {stage} shader code at '{path}'")]
    Unaligned { path: String, stage: ShaderStage },

    #[error("failed to create internal shader module for {stage} shader at '{path}'")]
    Creation {
        #[source]
        source: vk::Result,
        path: String,
        stage: ShaderStage,
    },
}

struct ShaderModuleInner {
    entry: String,
    stage: ShaderStage,
    shader_module: vk::ShaderModule,

    graphics_device: GraphicsDevice,
}

impl Drop for ShaderModuleInner {
    fn drop(&mut self) {
        unsafe {
            self.graphics_device
                .device()
                .destroy_shader_module(self.shader_module, None);
        }
    }
}

pub(crate) struct ShaderModule {
    inner: Arc<ShaderModuleInner>,
}

impl ShaderModule {
    pub(super) fn new(
        graphics_device: &GraphicsDevice,
        descriptor: &ShaderModuleDescriptor,
    ) -> Result<Self, ShaderModuleError> {
        let bytes = shader_compiler::compile(
            descriptor.path,
            descriptor.entry,
            descriptor.stage,
            OutputApi::Vulkan,
        )
        .map_err(|error| ShaderModuleError::Compilation {
            source: error,
            path: descriptor.path.to_owned(),
            stage: descriptor.stage,
        })?;

        let (prefix, code, suffix) = unsafe { bytes.align_to::<u32>() };
        if !prefix.is_empty() || !suffix.is_empty() {
            return Err(ShaderModuleError::Unaligned {
                path: descriptor.path.to_owned(),
                stage: descriptor.stage,
            });
        }

        let create_info = vk::ShaderModuleCreateInfo::default().code(code);

        let shader_module = unsafe {
            graphics_device
                .device()
                .create_shader_module(&create_info, None)
        }
        .map_err(|error| ShaderModuleError::Creation {
            source: error,
            path: descriptor.path.to_owned(),
            stage: descriptor.stage,
        })?;

        Ok(Self {
            inner: Arc::new(ShaderModuleInner {
                entry: descriptor.entry.to_owned(),
                stage: descriptor.stage,
                shader_module,

                graphics_device: graphics_device.clone(),
            }),
        })
    }

    pub(super) fn shader_module(&self) -> vk::ShaderModule {
        self.inner.shader_module
    }

    pub(crate) fn stage(&self) -> ShaderStage {
        self.inner.stage
    }

    pub(super) fn entry(&self) -> &str {
        &self.inner.entry
    }
}
